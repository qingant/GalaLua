#!/usr/bin/lua

module(..., package.seeall)

-- Driver to module mapping
local name_to_module = {
    MySQL = 'dbdmysql',
    PostgreSQL = 'dbdpostgresql',
    SQLite3 = 'dbdsqlite3',
    DB2 = 'dbddb2',
    Oracle = 'dbdoracle',
}

local string = require('string')

-- Returns a list of available drivers
-- based on run time loading
local function available_drivers()
    local available = {}

    for driver, modulefile in pairs(name_to_module) do
	local m, err = pcall(require, modulefile)

	if m then
	    table.insert(available, driver)
	end
    end

    -- no drivers available
    if table.maxn(available) < 1 then
	available = {'(None)'}
    end

    return available
end

local Cursor = {_connectInstance = nil,_statement = nil}

function Cursor:new(stm)
    if(stm) then
        obj = {};
        setmetatable(obj,self);
        self.__index = self;
        self._statement = stm;
        return obj;
    else
        error("the paramter is invalid");
    end
end

function Cursor:close()
    if self._statement then
       self._statement:close() 
    end
end

function Cursor:fetchone()
    if not self._statement then
        error("the cursor invalid")
    end
    return self._statement:fetch(true)
end

function Cursor:fetchmany(size)
    if size == 1 then
        return {self:fetchone()}
    elseif size >1 then
        local rowcount = size
	local rows = {}
	local record = self:fetchone()
	while record and (rowcount > 0) do
	    table.insert(rows,record)
	    rowcount = rowcount - 1
	    record = self:fetchone()
	end
	return rows
    else
        return self:fetchall()
    end
end

function Cursor:fetchall()
    local rows = {}
    local record = self:fetchone()
    while record do
        table.insert(rows,record)
         record = self:fetchone()
    end
    return rows    
end

local Connection = {_conn = nil}

function Connection:new(obj)
    obj = obj or {};
    setmetatable(obj,self);
    self.__index = self;
    return obj;
end

function Connection:ping()
    return self._conn:ping()
end

function Connection:autocommit(turn_on)
    return self._conn:autocommit(turn_on)
end

function Connection:close()
    return self._conn:close()
end

function Connection:prepare(sql)
    return self._conn:prepare(sql)
end

function Connection:commit()
    return self._conn:commit()
end

function Connection:rollback()
   return self._conn:rollback()
end

function Connection:execute(sql_string,...)
    if self._conn then
        local stm
	local err 
	stm,err = self:prepare(sql_string)
        if not stm then
	    error(err)
	end
        local success
	success,err = stm:execute(...)
	if not success then
	    error(err)
	end
	local cur = Cursor:new(stm)
	return cur
    else
       error("the connection object invalid") 
    end
end

 -- High level DB connection function
 -- This should be used rather than DBD.{Driver}.New
function connect(driver, ...)
    local modulefile = name_to_module[driver]

    if not modulefile then
        local available = table.concat(available_drivers(), ',')
	error(string.format("Driver '%s' not found. Available drivers are: %s", driver, available))
    end

    local m, err = pcall(require,modulefile)
    
    if not m then
	-- cannot load the module, we cannot continue
        local available = table.concat(available_drivers(), ',')
	error(string.format('Cannot load driver %s. Available drivers are: %s', driver, available))
    end

    local class_str = string.format('DBD.%s.Connection', driver)

    local connection_class = package.loaded[class_str]

    -- Calls DBD.{Driver}.New(...)
    --return connection_class.New(...)    
    local con = Connection:new();
    m,err = connection_class.New(...)
    if not m then
        error(err)
    end
    con._conn =  m   
    return con
end

-- Help function to do prepare and execute in 
-- a single step
--[=[function Do(dbh, sql, ...)
    local sth,err = dbh:prepare(sql)

    if not sth then
	return false, err
    end

    local ok, err = sth:execute(...)

    if not ok then
        return false, err
    end

    return sth:affected()
end
]=]

-- Lit drivers available on this system
function Drivers()
    return available_drivers() 
end
