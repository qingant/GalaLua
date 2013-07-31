module(...,package.seeall)

-- trim whitespace from both ends of string
function trim(s)
    return s:find'^%s*$' and '' or s:match'^%s*(.*%S)'
end
 
-- trim whitespace from left end of string
function ltrim(s)
    return s:match'^%s*(.*)'
end
 
-- trim whitespace from right end of string
function rtrim(s)
    return s:find'^%s*$' and '' or s:match'^(.*%S)'
end
-- delete '\0' character from ends of string
function rtrimz(s)
   return trim(string.gsub(s,"%z*$",""))
end
