local r=require("router")

for i,v in pairs(r) do print(i,v) end

print("****************************")
a=r.new("router")
for i,v in pairs(getmetatable(a)) do print(i,v) end

a:register("apple","bug","doctor",123,121)
a:register("app","bug","doctorRout",223,21)
a:delete("app")
print("************************\nfinding app:\n")
--ret=a:find_by_name("app")
function pitem(ret)
	for i,v in pairs(ret) do
		if type(v)=="table" then
			for k,g in pairs(v) do print(k,g) end
		else
			print(i,v)
		end
	end
end

--pitem(ret)

print("************************\nfinding bug:\n")
ret=a:find_by_field("bug")
for i,v in pairs(ret) do
	pitem(v)
end

a:finalizer()
