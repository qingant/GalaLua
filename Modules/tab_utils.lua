module(..., package.seeall)


local function update(tab1, tab2)
	for k,v in pairs(tab2) do
		tab1[k] = v
	end
	return tab1
end
