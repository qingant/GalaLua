

local core = require "md5.core"
local string = require "string"

module ("md5")

-- @param context String with original message.
-- @return String with the md5 hash value converted to hexadecimal digits
function sumhexa (context)
  context = core.sum(context)
  return (string.gsub(context, ".", function (c)
           return string.format("%02x", string.byte(c))
         end))
end
