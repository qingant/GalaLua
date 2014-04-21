--[[
	Author : Ma Tao
	Date   : 2014/04/14

	== Introduction ==
	Processes Pool

]]

module(..., package.seeall)

ppool = {}

function ppool:new(  )
	local o = {}
	setmeatatable(o, self)
	self.__index = self
	return o
end

function ppool:init( dispatcher, runnable, logger )
	self._max = 24
	self._min = 4
	self._idx = 0
	self._total = 0
	self._runnable = runnable
	self._dispatcher = dispatcher
	self._logger = logger
end
function ppool:_create_process(  )
	glr.spawn(self._runnable, "run", {index = self._idx + 1})
	self._idx = self._idx + 1
	self._total = self._total + 1
end
function ppool:init(  )
	-- create workers first
	for i=1,self._min do
		self:_create_process()
	end

	-- then create dispatcher
	glr.spawn(self._dispatcher, "run", {})

end
function ppool:main(  )
	while true do

	end
end


