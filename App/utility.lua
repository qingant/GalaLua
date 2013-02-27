
function repr(obj)
  local tobj = type(obj)
  local s = ""
  if tobj == "string" then
    s = string.format("%q", obj)
  elseif tobj == "number" or tobj == "boolean" then
    s = tostring(obj)
  elseif tobj == "nil" then
    s = "nil"
  elseif tobj == "table" then
    s = s  .. "{"
    for k,v in pairs(obj) do
      s = s .. "[" .. repr(k) .. "] = " .. repr(v) .. ", "
    end
    s = s  .. "}"
  else
    error("can't repr this object.")
  end
  return s  
end
