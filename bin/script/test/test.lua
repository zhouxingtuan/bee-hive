--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/6/1
-- Time: 上午12:12
-- To change this template use File | Settings | File Templates.
--

local math = math
local os = os
local require = require
local server_type = require("server_type")
local rpc = require("rpc")
local timer = require("timer")

local log_debug = log_debug
local log_error = log_error

local test = class("test")

rpc:addListener(test)

function test:onInitialize(pNode, param)
	self.m_pNode = pNode
	log_debug("onInitialize call once on init only")

end
function test:onRegister()
	log_debug("onRegister call on init or hotupdate")
	rpc:register(0x3001, self, self.CallTest)

	self:testMsg()
end
function test:onCloseConnect(pAccept, connType, connHandle)
	log_debug("onCloseConnect connType", connType, "connHandle", connHandle)

end
function test:onRouteRequest(uid, msgID)
	local desType,desID

	return desType,desID
end
function test:onQueryUID(connType, connHandle)
	local uid

	return uid
end
function test:onQueryAccessID(uid)
	local desID

	return desID
end
function test:onQueryClientID(uid)
	local desID

	return desID
end
function test:onDestroy()

end


-- ==================
function test:testMsg()
	timer:schedule(function()
		local request = {
			Test = "Hello World!";
		}
		local response = {}
		log_debug("testMsg call 0x3001")
		local result = rpc:request(server_type.test, 1, 0x3001, 100001, request, response, nil)
		log_debug("testMsg call 0x3001", result, "response", response)
	end, 2000, false)
end
function test:CallTest(request, response, rpcReq)
	log_debug("CallTest uid", rpcReq.uid, "request", request)
	response.Msg = "Welcome!"
end


return test
