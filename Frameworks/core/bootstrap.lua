--[[
    Author : Ma Tao
	Date   : 2014/06/22

	== Introduction ==
    Gala Application Framework Bootstrap
    Bootstrap服务：管理app
]]

module(..., package.seeall)

require "tab_utils"
local fmt = string.format
local pprint = require("pprint")
local rpc = require(_PACKAGE .. "rpc")
local logger = require(_PACKAGE .. "logger").logger
local kernel_app_name = "kernel"
local kernel_sup_name = fmt("%s.supervisor", kernel_app_name)
local bootstrap_name="bootstrap"

local function _get_free_bgpid() -- get free binded gpid
    local ps = glr.status.processes()
    for i = 32, 128 do
        if ps[i] == nil then
            return i
        end
    end
end

local app_sep="-"
local function get_app_module(app_name)
    local index=string.find(app_name,app_sep) or (#app_name + 1)
    return string.sub(app_name,1,index -1)
end

local function get_app_name(app_mod, inst_name)
    local app_inst_name=app_mod
    if inst_name then
        app_inst_name = fmt("%s%s%s", app_mod,app_sep, inst_name)
    end
    return app_inst_name
end

local function require_app(app_name)
    local app_mod=get_app_module(app_name)
    return require(fmt("%s.app", app_mod))
end

local app_mgr = {}
function app_mgr:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end
function app_mgr:init(kernel_sup,app_name,args,logger)
    
    self._logger=logger
    self.args = args
    self.app_name = app_name
    self.app_inst_name = app_name
    self._kernel_sup=kernel_sup

    self.app_module = require_app(app_name)
    self.app = self.app_module.app:new()
    self.app:init(self.args)

    self.supervisor_id = fmt("%s.supervisor", self.app_inst_name)

    self.app.log_path=self.app.log_path or self:default_app_log_path()
    
    self:start_app_supervisor()
    return self
end

function app_mgr:default_app_log_path()
    local log_path=string.format("%s/%s/",self.args.log_path,self.app_inst_name)
    os.execute("mkdir -p "..log_path)
    return log_path
end

function app_mgr:_start_pool(com)
    -- start pool
    print("start pool")
    local services = com.services or {}
    for i,v in ipairs(services) do
        self._supervisor:call("start_process", {
                                  process_type = "gen",
                                  process_params = {
                                      mod_name = v,
                                      parameters = {self.app_inst_name, com.pool_name, com.params}
                                  }
        })
    end
    self._supervisor:call("start_process", {
                process_type = "gen",
                process_params = {
                    mod_name = "core.pool",
                    parameters = {self.app_inst_name, com.pool_name, com.dispatcher, com.worker, com.params}
                }
    })

end

function app_mgr:_start_server(com)
    self._supervisor:call("start_process", {
                                  process_type = com.process_type or "gen",
                                  process_params = {
                                      mod_name = com.mod_name,
                                      parameters = {self.app_inst_name, com.params}
                                  }
                              })

end

--[[
@com: 有pool和server两种，当catagory为pool时必须包含以下字段{
        catagory = "pool",
        pool_name = "must set pool",
        dispatcher = "must set dispatcher",
        services = {"service 1","service 2"},
        worker = "worker module name",
        params = {}
       }，
     当catagory为server时必须包含以下字段{
        catagory = "server",
        process_type="gen"/"raw",
        mod_name="module name",
        params={}
      }
]]
function app_mgr:_start_component(com)
    com.params.log_path=self.app.log_path
    if  com.catagory == "server" then
        self:_start_server(com)
        -- TODO: error handling and logging
    elseif com.catagory == "pool" then
        self:_start_pool(com)
    end
end


function app_mgr:start_app_supervisor()
    if self:_is_kernel() then
        self._supervisor=self._kernel_sup
        return
    end

    local app_sup_start_arguments = {
        process_type = "gen",
        process_params = {
            mod_name = "core.supervisor",
            bind_gpid = _get_free_bgpid(),
            parameters = {self.supervisor_id, self.app.log_path},
        },
    }
    local rt = self._kernel_sup:call("start_process", app_sup_start_arguments)
    if rt.error then
        error(rt.error)
    end
    glr.time.msleep(30)          -- wait register
    local sup_client = rpc.create_client(self.supervisor_id)
    self._supervisor = sup_client
end

function app_mgr:_start_app()
    local app = self.app
    if app.on_load then
        app:on_load(self)
    end

    -- startup other services
    if app.components then
        for i, v in ipairs(app.components) do
            self:_start_component(v)
        end
    end

    if app.on_started then
        app:on_started(self)
    end
end

function app_mgr:_is_kernel()
    return self.app_module._NAME == "kernel.app"
end

function app_mgr:_stop_app()
    --TODO: deal with dependency
    local ok=self._supervisor:call("stop_all", {})

    if self:_is_kernel() then
        glr.kill(_ROOT_SUPERVISOR_GPID)
    else
        self._kernel_sup:call("stop", {process=self.supervisor_id})
    end
    --TODO: error handling

    if self.app.on_stoped then
        self.app:on_stoped(self)
    end
end

local bootstrap=rpc.server:new()
server=bootstrap
function bootstrap:new(process_name,args)
    local o={}
    setmetatable(o,self)
    self.__index=self

    self.init(o,bootstrap_name)
    self.init2(o,process_name,args)

    return o
end

function bootstrap:init2(process_name,args)
    self._apps={}
    self.args=args
    self._process_name=process_name

    self.log_path=string.format("%s/log/%s/",self.args.workdir,self._process_name)
    os.execute("mkdir -p " ..self.log_path)

    self.args.log_path=self.log_path
    self._logger=logger:new():init(self,string.format("%s/bootstrap.log",self.log_path))
    self._logger:info("bootstrap start")
    self:_start_kernel_app()
end

_ROOT_SUPERVISOR_GPID = 3
function bootstrap:_start_kernel_supervisor()
    self._kernel_sup=rpc.create_server{mod_name = "core.supervisor", 
                                       bind_gpid = _ROOT_SUPERVISOR_GPID,
                                       parameters = {kernel_sup_name,self.log_path}}
end

function bootstrap:_start_kernel_app()
    self:_start_kernel_supervisor()
    self:_start_app("kernel")
end

function bootstrap:start_app(params,desc)
    local app_name=params.app_name
    local auto=params.auto_dep

    local ok,emsg=pcall(self._start_app,self,app_name,params.kwargs)
    if ok then
        return self.ok
    end
    self._logger:error("start app error:%s",emsg)
    return nil,emsg

end

--[[
start dependence
--]]
function bootstrap:_start_deps(deps)
    local name,kwargs
    if deps then
        for i,dep in ipairs(deps) do
            if type(dep)=="table" then
                name = get_app_name(dep.app_name,dep.inst_name)
                kwargs = dep.kwargs
            else
                name = dep
            end
            if name~=nil then
                self:_start_app(name,kwargs)
                self._logger:info("start dependent app:%s succ",name)
            end
        end
    end
end

function bootstrap:_start_app(app_name,kwargs)
    self._logger:info("start app:%s", app_name)

    --TODO: app已启动
    if self._apps[app_name] then
        return "alreadly started"
    end

    local app=require_app(app_name).app
    
    self:_start_deps(app.deps)

    local argv=table.dup(self.args)
    if type(kwargs)=="table" then
        table.update(argv,kwargs)
    end

    if not app.is_dummy_app then
        local app_obj=app_mgr:new():init(self._kernel_sup, app_name, argv,self._logger)
        app_obj:_start_app(app_obj)
        self._apps[app_name]=app_obj
    else
        --TODO: stop dummy app
    end
end


--[[
TODO:检查依赖关系
]]
function bootstrap:stop_app(params,desc)
    local app_inst_name=params.app_name

    self._logger:info("stop app:%s", app_inst_name)

    local app=self._apps[app_inst_name]
    if app then
        app:_stop_app()
        self._apps[app_inst_name]=nil
    end
    return self.ok
end

local app_cli={}
client=app_cli
function app_cli:new()
    local o={}
    setmetatable(o,self)
    self.__index=self
    return self
end
function app_cli:init(cli)
    self._bootstrap=cli or rpc.create_client(bootstrap_name)
    return self
end

--[[
TODO: 错误处理
]]
function app_cli:start_app(app_name,inst_name,kwargs)
    return self._bootstrap:call("start_app",{app_name=get_app_name(app_name,inst_name),kwargs=kwargs})
end
function app_cli:stop_app(app_name,inst_name)
    return self._bootstrap:call("stop_app",{app_name=get_app_name(app_name,inst_name)})
end


_BOOTSTRAP_GPID=1
-- system entry
function main()
    local workdir=glr.get_option("workdir") or os.getenv("HOME")
    glr.set_option("workdir",workdir)
    local init_app=string.split(glr.get_option("init-app"),",")

    local process_name=glr.get_option("name") or init_app[1]

    local boot=rpc.create_server{mod_name = "core.bootstrap", bind_gpid = _BOOTSTRAP_GPID, parameters = {process_name,glr.get_options()}}
   
    local bs=app_cli:new():init(boot)

    for _,app_name in ipairs(init_app) do
        pprint.pprint(bs:start_app(app_name))
    end

    if glr.get_option("interactive") then
        local grepl = require("grepl").GRepl
        local g = grepl:new()
        g:mockEnv({"start_app", start_app},
            {"stop_app", stop_app})
        g:main()
    end
end
if ... == "__main__" then
    main()
end
