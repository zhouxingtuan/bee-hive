--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/5/20
-- Time: 下午1:55
-- To change this template use File | Settings | File Templates.
--

--[[
class Node
{
    uint32 startTimer(uint32 callbackID, int64 timeCount);			        // 开始一个计时器
    void removeTimer(uint32 handle);								        // 移除计时器
    void changeTimer(uint32 handle, int64 timeCount);				        // 更改计时器的时间
    int64 getTimerLeft(uint32 handle);									        // 获取计时器剩余时间

	bool sendMessage(uint32 moduleType, uint32 nodeID, const char* ptr, uint32 length);

	uint32 hash(const char* str, uint32 length);
	void encrypt(const char* str, uint32 length, std::string& result);
	void decrypt(const char* str, uint32 length, std::string& result);

	uint32 getNodeID(void){ return m_nodeID; }
	void setNodeID(uint32 nodeID){ m_nodeID = nodeID; }
	uint32 getModuleType(void) const;
	uint32 getModuleIndex(void) const;
	uint32 getModuleHandle(void) const;
};




-- ]]

local tostring = tostring
local debug = debug
local error_function = print
local print_error = function(msg, traceMsg)
	error_function("----------------------------------------")
	error_function("LUA ERROR: " .. tostring(msg) .. "\n")
	error_function(traceMsg)
	error_function("----------------------------------------")
end
local register_error
function set_register_error_function(callback)
	register_error = callback
end

local g_pNode = g_pNode
local nodeID = g_pNode:getNodeID()
local moduleType = g_pNode:getModuleType()
local moduleIndex = g_pNode:getModuleIndex()

local getTimeStringUS = getTimeStringUS
local t = getTimeStringUS()
local arr = { t, moduleType, moduleIndex, nodeID }
local str = table.concat(arr, "_")
local seed = g_pNode:hash(str, #str)
local log_str = "current seed "..seed .. "\n"
g_pNode:log(log_str)
math.randomseed(seed)

-- traceback
function __G__TRACKBACK__(msg)
	local traceMsg = debug.traceback()
	print_error(msg, traceMsg)
	if register_error ~= nil then
		return register_error(msg, traceMsg)
	end
end
local __G__TRACKBACK__ = __G__TRACKBACK__
local xpcall = xpcall

function write_error_info(...)
	local args = {... }
	local str = table.concat(args, " ")
	str = str .. "\n"
	local file = "require_error.log"
	local outfile = io.open(file, "a")
	outfile:write(str)
	outfile:flush()
	outfile:close()
end
local write_error_info = write_error_info

--old_require = require
--local old_require = old_require
--require = function(file)
--	local result,err = pcall(old_require, file)
--	if not result then
--		write_error_info("require failed", file, err)
--	else
--		return err
--	end
--end

local rpc

local initEnvironment = function()
	local base_path = {
		"base";
		"pb";
	}
	local config = require("config")
	table.insert(base_path, config.moduleName)
	local package = package
	package.path = "./?.lua;./script/?.lua;"
	package.cpath = "./?.so;./lib/?.so;"
	for _,p in ipairs(base_path) do
		p = "script/"..p
		package.path = package.path .. "./"..p.."/?.lua;"
		package.cpath = package.cpath .. "./"..p.."/?.so;"
	end
	local log = require("log")
	for _,p in ipairs(base_path) do
		p = "script."..p..".init"
		require(p)
	end
	log.level = config.logLevel or "trace"
	log.maxFileSize = config.maxFileSize or log.maxFileSize
	error_function = log_error
	rpc = require("rpc")
end

function onInitialize(pNode, param)
	return xpcall(function()
		-- 打开Lua的日志文件
		local config = require("config")
		local nodeID = pNode:getNodeID()
		local moduleType = pNode:getModuleType()
		local moduleIndex = pNode:getModuleIndex()
		local arr = {moduleType, moduleIndex, nodeID}
		local str = table.concat(arr, "_")
		local moduleName = config.moduleName or "bee"
		local log_file_name = moduleName.."_"..str..".log"
		local log = require("log")
		log.open(log_file_name)

		return rpc:onInitialize(pNode, param)
	end, __G__TRACKBACK__)
end
function onDestroy()
	return xpcall(function() return rpc:onDestroy() end, __G__TRACKBACK__)
end
function onReceiveAccept(pAccept, buffer, command, callbackID, desType, desIndex, resType, resIndex, message, uid)
	return xpcall(function() return rpc:onReceiveAccept(pAccept, buffer, command, callbackID, desType, desIndex, resType, resIndex, message, uid) end, __G__TRACKBACK__)
end
function onCloseAccept(pAccept)
	return xpcall(function() return rpc:onCloseAccept(pAccept) end, __G__TRACKBACK__)
end
function onReceiveHttp(pHttp, buffer, command, callbackID, desType, desIndex, resType, resIndex, message, uid)
	return xpcall(function() return rpc:onReceiveHttp(pHttp, buffer, command, callbackID, desType, desIndex, resType, resIndex, message, uid) end, __G__TRACKBACK__)
end
function onCloseHttp(pHttp)
	return xpcall(function() return rpc:onCloseHttp(pHttp) end, __G__TRACKBACK__)
end
function onCurlResponse(isRequestOK, callbackID, buffer)
	return xpcall(function() return rpc:onCurlResponse(isRequestOK, callbackID, buffer) end, __G__TRACKBACK__)
end
function onTimerUpdate(callbackID)
	return xpcall(function() return rpc:onTimerUpdate(callbackID) end, __G__TRACKBACK__)
end

-- 初始化环境变量配置
initEnvironment()



