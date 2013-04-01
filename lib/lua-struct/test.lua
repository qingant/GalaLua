local struct = require "struct"

str = struct.pack(">BiIl4Lc0",34,45,46,-47,48,"123456789")
a,b,c,d,e,h = struct.unpack(">BiIl4Lc9",str)
print(a,b,c,d,e,h,struct.size(">BiIl4Lc9"))
