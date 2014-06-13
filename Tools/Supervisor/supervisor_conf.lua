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


function create(path)
    --create and open env
    local path=path or PATH 

    os.execute("mkdir -p "..path)
    return mdb._create_env(path) 
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
    -- <supervisord stderr=logfile stdout=logfile host="" port="" module="" gar="path" arg=""> </supervisord>
    --  <group1>
    --      <module_name>
    --          <index stderr=logfile stdout=logfile host="" port="" entry="main" valid=true gar="path" arg=""></index>
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
    function watch_conf:save(conf_entry,group)
        --TODO:default value
        local group=group or conf_entry.group or defaultGroup
        local host=conf_entry.host
        local port=conf_entry.port
        local module_name=conf_entry.module_name
        local index=conf_entry.index  or 0
        local stderr=conf_entry.stderr  or ""
        local stdout=conf_entry.stdout  or ""
        local gar_path=conf_entry.gar or ""
        local arg=conf_entry.arg or ""
        local valid=conf_entry.valid or true

        if not (host and port and module_name) then 
            error("Must pass host, port ,module_name")
            return 
        end

        local ref_name=string.format("%s::%s",host,port)
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
                                index_node:add_attrib("arg",arg)
                                index_node:add_attrib("valid",valid)
                                
--                                print(ref_name)
                                self.ref_root:_add_ref(ref_name,index_node)

                                self.db:commit()
                            end
                           )
    end

    function watch_conf:save_supervisord(conf_entry)
        --TODO:default value
        local host=conf_entry.host
        local port=conf_entry.port
        local module_name=conf_entry.module_name
        local stderr=conf_entry.stderr  or ""
        local stdout=conf_entry.stdout  or ""
        local gar_path=conf_entry.gar or ""
        local arg=conf_entry.arg or ""

        if not (host and port and module_name) then 
            error("Must pass host, port ,module_name")
            return 
        end

        local ref_name=string.format("%s::%s",host,port)
        return self.db:with(function ()

                                local sup=self.root:add_node("supervisord")
                                sup:add_attrib("stderr",stderr)
                                sup:add_attrib("stdout",stdout)
                                sup:add_attrib("host",host)
                                sup:add_attrib("port",port)
                                sup:add_attrib("module",module_name)
                                sup:add_attrib("gar",gar_path)
                                sup:add_attrib("arg",arg)
                                
                                self.ref_root:_add_ref(ref_name,sup)

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

    function watch_conf:find_supervisord()
        return self.db:withReadOnly(function ()
                                        local ret={}
                                        --XXX::xpath return an empty table if not existed
                                        local w=self.root:get_child("supervisord")

                                        if w then
                                            return w:get_attrib()
                                        end
                                    end
                                   )
    end

    function watch_conf:delete(v)
        error("not impletement!!!")
        return self.db:with(function ()
                                
                            end
                           )
    end

    function watch_conf:import()
        error("not impletment!")
    end

    return watch_conf
end



if ...=="__main__" then
    local _env=create()
    _conf=watchConf(_env)
    pprint.pprint(_conf:find_supervisord())
--    _conf:clear()

    local entry={host="127.0.0.1",port=3111,module_name="test",index=2,entry="main"}
    _conf:save(entry)
    print("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC")

--    _conf:import()
--    _conf:to_xml()
    _conf:show()
    _conf:close()
end
