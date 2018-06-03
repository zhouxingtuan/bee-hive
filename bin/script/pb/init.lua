--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/8/6
-- Time: 下午10:11
-- To change this template use File | Settings | File Templates.
--

local pbfiles = {
	"rpc.pb";
}

local protobuf = require("protobuf")

for _,name in ipairs(pbfiles) do
	local file_name = "script/pb/"..name
	protobuf.register_file(file_name)
end

return pbfiles
