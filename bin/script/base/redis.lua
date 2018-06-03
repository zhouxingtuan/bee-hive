--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 15/4/10
-- Time: 下午2:28
-- To change this template use File | Settings | File Templates.
--

local tonumber = tonumber
local string = string
local table = table
local pairs = pairs
local ipairs = ipairs
local unpack = unpack
local rpc = require("rpc")
local timer = require("timer")
local log_debug = log_debug
local log_error = log_error
local log_info = log_info

local redis = class("redis")
-- 创建使用new，销毁使用dtor函数
function redis:ctor()
	local m_pDBRedis = rpc:getNode():createRedis()
	self.m_pDBRedis = m_pDBRedis
	-- 设定计时器
	self.m_coInfo = timer:schedule(function()
		self:tryPing()
	end, 5000, true)
end
function redis:dtor()
	self:release()
	local m_coInfo = self.m_coInfo
	if m_coInfo ~= nil then
		self.m_coInfo = nil
		timer:unschedule(m_coInfo)
	end
end
function redis:tryPing()
	self:checkReconnect()
	local m_pDBRedis = self.m_pDBRedis
	if not m_pDBRedis:isAvailable() then
		return
	end
	local arr = { "PING" }
	local result = m_pDBRedis:command(false, arr)
	if result ~= "PONG" then
		log_debug("PING failed result", result)
		return self:checkReconnect()
	end
end
function redis:checkReconnect()
	local m_pDBRedis = self.m_pDBRedis
	if not m_pDBRedis:isAvailable() then
		local m_connParam = self.m_connParam
		if m_connParam ~= nil then
			return self:connect(unpack(m_connParam))
		end
		return
	end
end
function redis:isAvailable()
	local m_pDBRedis = self.m_pDBRedis
	if m_pDBRedis == nil then
		return false
	end
	return m_pDBRedis:isAvailable()
end
function redis:release()
	local m_pDBRedis = self.m_pDBRedis
	if m_pDBRedis ~= nil then
		self.m_pDBRedis = nil
		rpc:getNode():removeRedis(m_pDBRedis)
	end
end
function redis:connect(ip, port, pwd, index)
	pwd = pwd or ""
	index = index or 0
	self.m_connParam = {ip, port, pwd, index}
	local m_pDBRedis = self.m_pDBRedis
	local result = m_pDBRedis:connectRedis(ip, port)
    log_info("connect redis result", result, ip, port)
	if pwd ~= "" and result then
		result = m_pDBRedis:identifyRedis(pwd)
	end
	if result then
		if index > 0 then
			local arr = { "SELECT", index }
			m_pDBRedis:command(true, arr)
		end
		return true
    end
    log_error("identify redis failed", ip, port)
	return result
end
-- 同时进行多个请求时使用该接口
function redis:appendCommand(paramArr)
	self.m_pDBRedis:appendCommand(paramArr)
end
function redis:getReply()
	return self.m_pDBRedis:getReply()
end
-- 单个请求字符串
function redis:command(cmd, skipResult)
    local paramArr = { cmd }
    return self:executeQuery(paramArr, skipResult)
end
-- 特化请求接口封装
function redis:incr(key, skipResult)
    local paramArr = { "INCR", key }
    return self:executeQuery(paramArr, skipResult)
end
function redis:incrby(key, value, skipResult)
    local paramArr = { "INCRBY", key, value }
	return self:executeQuery(paramArr, skipResult)
end
function redis:del(keyArr, skipResult)
    local paramArr = { "DEL" }
	for _,k in ipairs(keyArr) do
		table.insert(paramArr, k)
	end
	return self:executeQuery(paramArr, skipResult)
end
function redis:hdel(hash, keyArr, skipResult)
    local paramArr = { "HDEL", hash }
	for _,k in ipairs(keyArr) do
		table.insert(paramArr, k)
	end
	return self:executeQuery(paramArr, skipResult)
end
function redis:mset(data, skipResult)
    local paramArr = { "MSET" }
    for k,v in pairs(data) do
        table.insert(paramArr, k)
        table.insert(paramArr, v)
    end
	return self:executeQuery(paramArr, skipResult)
end
function redis:msetnx(data, skipResult)
    local paramArr = { "MSETNX" }
    for k,v in pairs(data) do
        table.insert(paramArr, k)
        table.insert(paramArr, v)
    end
    return self:executeQuery(paramArr, skipResult)
end
function redis:mget(keyArr)
    local paramArr = { "MGET" }
    for _,k in ipairs(keyArr) do
		table.insert(paramArr, k)
	end
	return self:executeQuery(paramArr, false)
end
function redis:hmset(hash, data, skipResult)
    local paramArr = { "HMSET", hash }
    for k,v in pairs(data) do
        table.insert(paramArr, k)
        table.insert(paramArr, v)
    end
	return self:executeQuery(paramArr, skipResult)
end
function redis:hmget(hash, keyArr)
    local paramArr = { "HMGET", hash }
	for _,k in ipairs(keyArr) do
		table.insert(paramArr, k)
	end
	return self:executeQuery(paramArr, false)
end
function redis:hgetall(hash)
    local paramArr = { "HGETALL", hash }
	return self:executeQuery(paramArr, false)
end
function redis:set(key, value, skipResult)
    local paramArr = { "SET", key, value }
	return self:executeQuery(paramArr, skipResult)
end
function redis:setnx(key, value, skipResult)
    local paramArr = { "SETNX", key, value }
	return self:executeQuery(paramArr, skipResult)
end
function redis:get(key)
    local paramArr = { "GET", key }
	return self:executeQuery(paramArr, false)
end
function redis:hset(hash, key, value, skipResult)
    local paramArr = { "HSET", hash, key, value }
	return self:executeQuery(paramArr, skipResult)
end
function redis:hsetnx(hash, key, value, skipResult)
    local paramArr = { "HSETNX", hash, key, value }
	return self:executeQuery(paramArr, skipResult)
end
function redis:hget(hash, key)
    local paramArr = { "HGET", hash, key }
	return self:executeQuery(paramArr, false)
end
function redis:hincrby(hash, key, value, skipResult)
    local paramArr = { "HINCRBY", hash, key, value }
	return self:executeQuery(paramArr, skipResult)
end
function redis:exists(key)
	local paramArr = {"EXISTS", key}
	return self:executeQuery(paramArr, false)
end
function redis:sadd(key, keyArr)
	local paramArr = { "SADD", key }
	for _,k in ipairs(keyArr) do
		table.insert(paramArr, k)
	end
	return self:executeQuery(paramArr, false)
end
function redis:srem(key, keyArr)
	local paramArr = { "SREM", key }
	for _,k in ipairs(keyArr) do
		table.insert(paramArr, k)
	end
	return self:executeQuery(paramArr, false)
end
function redis:srandmember(key, count)
	local paramArr = { "SRANDMEMBER", key, count }
	return self:executeQuery(paramArr, false)
end
-- paramArr 中所有的参数会被转换成string类型
function redis:executeQuery(paramArr, skipResult)
    if #paramArr == 0 then
		log_error("executeQuery #paramArr == 0")
		return
	end
	skipResult = skipResult or false
	return self.m_pDBRedis:command(skipResult, paramArr)
end

function redis:rpush(key,value,skipResult)
	local paramArr = { "RPUSH", key, value }
	return self:executeQuery(paramArr, skipResult)
end

return redis


