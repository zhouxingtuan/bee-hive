--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/8/23
-- Time: 下午9:45
-- To change this template use File | Settings | File Templates.
--


local log_debug = log_debug
local log_error = log_error

local rapidjson = require("rapidjson")

local rpc = require("rpc")

local user = class("user")

rpc:addListener(user)
-- 模块初始化函数
function user:onInitialize(pNode, param)
	-- 注册cmd和处理函数
	log_debug("onInitialize", param)

	local test = {
		a = 123,
		b = "2323",
		c = {
			{t=123, v=444},
			{t=125, v=555},
		},
		d = "1234343434";
	}
	log_debug(test)
	local buf = rapidjson.encode(test)
	log_debug("buf", buf)
	local data = rapidjson.decode(buf)
	log_debug(data)
end
-- 模块销毁函数
function user:onDestroy()
	log_debug("onDestroy")
end

-- 注册消息
function user:onRegister()
    rpc:register(0x1001, self, self.userLogin)
end

function user:userLogin(request, response, rpcReq)
	log_debug("userLogin", request)
end

return user
