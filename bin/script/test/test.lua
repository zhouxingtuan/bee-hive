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
local rpc = require("rpc")

local log_debug = log_debug
local log_error = log_error

local test = class("test")

rpc:addListener(test)

function test:onInitialize(pNode, param)
	self.m_pNode = pNode

end
function test:onDestroy()

end



return test
