module(..., package.seeall)
local lom = require ("lxp.lom")
local etree = require ("etree")
local xpath = require ("xpath")

--find path
function FindPath(element, path)
   return xpath.selectNodes(element, path)
end

--find text
function Find(element, tag)
end
   
--oper parse
function OperParse(xmlstr)
    --xmlstr = xmlstr:sub(9)
    local element = etree.fromstring(xmlstr)
    local ret = {}
    ret["ErrorCode"] = FindPath(element, "/AFAPlatform/Response/ErrorCode")[1][1]
    ret["ErrorMsg"] = FindPath(element, "/AFAPlatform/Response/ErrorMsg")[1][1]
    if ret["ErrorCode"] == "0000" then
        ret["Result"] = "1"
    else
        ret["Result"] = "0"
    end
    return ret
end

--status parse
function StatusParse(xmlstr)
    --xmlstr = xmlstr:sub(9)
    local element = etree.fromstring(xmlstr)
    local ret = {}
    grps = FindPath(element, "/AFAPlatform/AFASTATUS/AFAGRPSTATUS")
    if #grps ~= 0 then
        ret["PlatStatus"] = "1"
        ret["Group"] = {}
        for _, group in pairs(grps) do
            local groupinfo = {}
            groupinfo["ID"] = FindPath(group, "/AFAGRPSTATUS/GroupID/text()")[1]
            print(groupinfo["ID"])
            groupinfo["Name"] = FindPath(group, "/AFAGRPSTATUS/Alias/text()")[1]
            print(groupinfo["Name"])
            groupinfo["Status"] = FindPath(group, "/AFAGRPSTATUS/Usable/text()")[1]
            print(groupinfo["Status"])
            groupinfo["LSR"] = {}
            chllist = FindPath(group, "/AFAGRPSTATUS/AFALSRSTALST/AFALSRSTATUS/CHL")
            statuslist = FindPath(group, "/AFAGRPSTATUS/AFALSRSTALST/AFALSRSTATUS/LBI/Usable")
            for idx, value in pairs(chllist) do
                chl = value[1]
                status = statuslist[idx][1]
                print(chl, status)
                groupinfo["LSR"][#groupinfo["LSR"]+1] = { ["CHL"] = chl, ["Status"] = status}
            end
            ret["Group"][#ret["Group"]+1] = groupinfo
        end
    else
        ret["PlatStatus"] = "0"
    end
    return ret 
end


