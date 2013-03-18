local r=require("router")

for i,v in pairs(r) do print(i,v) end

print("****************************")
a=r.new("router")
for i,v in pairs(getmetatable(a)) do print(i,v) end

a:register("apple","bug","doctor",123,121,"aaa","bbb","This is userdata")
a:register("app","bug","doctorRout",223,21,"aaas","ssss","This is userdata")
a:register("how long are you? longer than 24 char?","bug","doctorRout",223,21,"aaas","ssss","This is userdata")
--a:delete("app")
print("************************finding app:\n")
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

--print("************************finding bug:\n")
--ret=a:find_by_field("bug")
--for i,v in pairs(ret) do
--	pitem(v)
--    print("-------------\n")
--end
--
--print("************************finding dev_type aaas:\n")
--ret=a:find_by_dev_type("aaas")
--for i,v in pairs(ret) do
--	pitem(v)
--    print("-------------\n")
--end
--print("************************finding app_type ssss:\n")
--ret=a:find_by_app_type("ssss")
--for i,v in pairs(ret) do
--	pitem(v)
--    print("-------------\n")
--end
ret=a:find("","display")
for i,v in pairs(ret) do
	pitem(v)
    print("-------------\n")
end


a:finalizer()
