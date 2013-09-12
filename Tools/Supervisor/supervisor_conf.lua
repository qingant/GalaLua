--[[
    watch_conf using ldap

    所有的find*函数返回结果都是
    ret={
            [1]={ item },
            [2]={ item },
            [3]={ item },
            ...
        }
]]
module(...,package.seeall)

local mdb = require "mdb"
local pprint = require "pprint"
local os = require "os"
local mdb=mdb.mdb
local db_path=require "db_path"

Watch="Watch"
defaultGroup="wg1"
PATH=db_path.supervisor

local function isvalid_ip(ip)
    local ip=tostring(ip)
    local a,b,c,d=string.match(ip,"^(%d+)%.(%d+)%.(%d+)%.(%d+)$") 
    if a and tonumber(a)<=255 and tonumber(b)<=255 and 
       tonumber(c)<=255 and tonumber(d)<=255 then

       return true
    end

    return false
end

local function isvalid_port(port)
    local port=tonumber(port)
    if port and port>0 and port <65535 then
        return true
    end
    return false
end

local function isvalid_addr(host,port)
    function valid()
        if isvalid_ip(host) and isvalid_port(port) then
            return true
        end
    end

    local ok,ret=pcall(valid)
    if ok then
        return ret
    end
end


function create_with_path(path)
    --create and open env
    
    --FIXME::make it more elegant???
    os.execute("mkdir -p "..path)
    return mdb._create_env(path) 
end

function create()
    --create and open env
    
    --FIXME::make it more elegant???
    os.execute("mkdir -p "..PATH)
    return mdb._create_env(PATH) 
end

local data={db_type="lmdb"}
function data:new(o)
    local o= o or {}
    setmetatable(o,self)
    self.__index=self
    data.init(o)
    return o
end

function data:init()
    assert(self.env,"Not a valid env")
--    if not self.env then
--        self.env=create(self.path)
--    end
    self.db=mdb:new()
    self.db:init(self.env)
    self.root=self.db:get_root(self.root_name)
--    pprint.pprint(self)
end

function data:close()
    self.db:close()
end

function data:to_xml()
    return self.db:with(function ()
                            return self.root:to_xml()
                        end
                        )
end

function data:show(xpath)
    return self.db:withReadOnly(function ()
                                    node= xpath and self.root:xpath(xpath) or self.root 
                                    if node then 
                                        node:show()
                                    end
                                end
                               )
end

function watchConf(env)
    --[[
    --
    --<watch>
    --  <group1>
    --      <module_name>
    --          <index stderr=logfile stdout=logfile host="" port="" entry="main" valid=true gar="path"></index>
    --      </module_name>
    --  </group1>
    --</watch>
    --<idref>
    --   <host::port>
    --      reffer to the specify host and post entry
    --   </host::port>
    --</idref>
    --
    --XXX:if the gar path is not absolute path, db_path.gar_path will be "PWD"
    --]]
    local watch_conf=data:new({env=env,root_name=Watch})
    function watch_conf:init()
        self.ref_root=self.db:get_root("idref")
    end
    watch_conf:init()

    function watch_conf:show()
        self.db:withReadOnly(function ()
                                print("ref........")
                                self.ref_root:show()
                                print("root........")
                                self.root:show()
                             end
                            )
    end

    function watch_conf:clear()
        self.db:with(function ()
                         self.ref_root:remove()
                         self.root:remove()
                         self.db:commit()
                      end
                     )
    end

    --[[
    --@conf_entry: {
    --              host,
    --              port,
    --              module_name,
    --              index,
    --              group,
    --              stderr,
    --              stdout,
    --              gar,
    --              }
    --
    --]]
    function watch_conf:save(conf_entry)
        --TODO:default value
        local group=conf_entry.group or defaultGroup
        local host=conf_entry.host
        local port=conf_entry.port
        local module_name=conf_entry.module_name
        local index=conf_entry.index  or 0
        local stderr=conf_entry.stderr 
        local stdout=conf_entry.stdout
        local gar_path=conf_entry.gar or ""
        local valid=conf_entry.valid or true

        if not (host and port and module_name) then 
            print("Must pass host, port ,module_name")
            return 
        end

        return self.db:with(function ()

                                local group_node=self.root:add_node(group)
                                local mod_node=group_node:add_node(module_name)
                                local index_node=mod_node:add_node(index)

                                index_node:add_attrib("stderr",stderr)
                                index_node:add_attrib("stdout",stdout)
                                index_node:add_attrib("host",host)
                                index_node:add_attrib("port",port)
                                index_node:add_attrib("module",module_name)
                                index_node:add_attrib("index",index)
                                index_node:add_attrib("group",group)
                                index_node:add_attrib("gar",gar_path)
                                index_node:add_attrib("valid",valid)
                                
                                local ref_name=string.format("%s::%s",host,port)
--                                print(ref_name)
                                self.ref_root:_add_ref(ref_name,index_node)

                                self.db:commit()
                            end
                           )
    end

    function watch_conf:find_by_id(id)
        return self.db:withReadOnly(function ()
                                        local ret={}
                                        --XXX::xpath return an empty table if not existed
                                        local w=self.ref_root:xpath(id)
                                        for id,node in pairs(w) do
                                            ret[#ret+1]=node:get_attrib()
                                        end
                                        return ret
                                    end
                                   )
    end

    --group:must passed
    --module_name:lsr or svc (optional)
    --index:0,1,2,...
    --find(group1,lsr,0)  -->find lsr0000 in group1
    function watch_conf:find(group,module_name,index)
        return self.db:withReadOnly(function ()
                                        local ret={}
                                        local xpath=group or "*"
                                        xpath=xpath.."/"..(module_name or "*")
                                        xpath=xpath.."/"..(index or "**")
                                        print(xpath)
                                        local w=self.root:xpath(xpath)

                                        for i,node in pairs(w) do
                                            ret[#ret+1]=node:get_attrib()
                                        end
                                        return ret
                                    end
                                   )
    end
    function watch_conf:delete(v)
        error("not impletement!!!")
        return self.db:with(function ()
                                
                            end
                           )
    end


    function watch_conf:raw_import()

        local mdb_path =db_path.config
        local db=mdb:new():init(create_with_path(mdb_path))
        local _conf=self

        db:withReadOnly(function (db) 
            local root=db:get_root("Configure"):get_child("Contents")
            local lsr=root:get_child("LSRList")

            for i,ch in pairs(lsr:get_child()) do
                local host=ch:xpath("Address/IP")[1]:get_value()[1]
                local port=ch:xpath("Address/Port")[1]:get_value()[1]

                if isvalid_addr(host,port) then
                    local index=i
                    local group=defaultGroup
                    local entry="main"
                    local module_name="lsr"
                    local stderr=string.format("%s%.4d-stderr.log",module_name,index)
                    local stdout=string.format("%s%.4d-stdout.log",module_name,index)

                    local e={host=host,port=port,module_name=module_name,index=index,
                    stderr=stderr,stdout=stdout,group=group,entry=entry}
                    _conf:save(e)
                end
            end
            local svc=tonumber(root:xpath("Base/SVCLimits")[1]:get_value()[1])
            local svc_port=tonumber(root:xpath("Base/SVCPort")[1]:get_value()[1])
            for i=1,svc do
                local host="127.0.0.1"
                local port=svc_port+i
                local index=i
                local group=defaultGroup
                local entry="main"
                local module_name="svc"
                local stderr=string.format("%s%.4d-stderr.log",module_name,index)
                local stdout=string.format("%s%.4d-stdout.log",module_name,index)

                local e={host=host,port=port,module_name=module_name,index=index,
                stderr=stderr,stdout=stdout,group=group,entry=entry}
                _conf:save(e)
            end
        end)
        db:close()
    end


    function watch_conf:import()
        --clear all supervisor configure before import 
        self:clear()
        self:raw_import()
    end


    return watch_conf
end



if ...=="__main__" then
    local _env=create()
    _conf=watchConf(_env)
--    _conf:show()
    _conf:clear()

    entry={host="-1.0.0.0",group=defaultGroup,module_name="lsr",entry="main",index=2}
    _conf:save(entry)
    print("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC")

--    _conf:import()
--    _conf:to_xml()
    _conf:show()
    _conf:close()
end
