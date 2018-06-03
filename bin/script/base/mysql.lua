--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 15/4/10
-- Time: 下午2:28
-- To change this template use File | Settings | File Templates.
--

local _G = _G
local string = string
local pairs = pairs
local ipairs = ipairs
local table = table
local type = type
local unpack = unpack
local os = os
local tostring = tostring
local log_debug = log_debug
local log_error = log_error
local timer = require("timer")
local rpc = require("rpc")
local queue = require("queue")

local mysql = class("mysql")
-- 创建使用new，销毁使用dtor函数
function mysql:ctor()
	-- 设定计时器
	local m_pDBMysql = rpc:getNode():createMysql()
	self.m_pDBMysql = m_pDBMysql

	self.m_coInfo = timer:schedule(function()
		self:tryPing()
		self:checkToSave()
	end, 5000, true)
	self.m_hash_queue = queue.new()
end
function mysql:dtor()
	self:release()
	local m_coInfo = self.m_coInfo
	if m_coInfo ~= nil then
		self.m_coInfo = nil
		timer:unschedule(m_coInfo)
	end
	self.m_hash_queue = nil
end
function mysql:release()
	local m_pDBMysql = self.m_pDBMysql
	if m_pDBMysql ~= nil then
		self.m_pDBMysql = nil
		rpc:getNode():removeMysql(m_pDBMysql)
	end
end
function mysql:tryPing()
	self:checkReconnect()
	local m_pDBMysql = self.m_pDBMysql
	if not m_pDBMysql:isAvailable() then
		return
	end
	local result = m_pDBMysql:pingMysql()
	if not result then
		log_debug("pingMysql failed result", result)
		return self:checkReconnect()
	end
end
function mysql:checkReconnect()
	local m_pDBMysql = self.m_pDBMysql
	if not m_pDBMysql:isAvailable() then
		local m_connParam = self.m_connParam
		if m_connParam ~= nil then
			return self:connect(unpack(m_connParam))
		end
		return
	end
end
function mysql:checkToSave()
    -- 把缓存的数据全部保存
	local m_hash_queue = self.m_hash_queue
	local sqlArr = m_hash_queue:copyArr()
	if #sqlArr > 0 then
		if self:executeQuery(sqlArr, true) then
			m_hash_queue:clear()
		end
	end
end
-- 公共方法
function mysql:connect(ip, user, pwd, db, port)
	local m_pDBMysql = self.m_pDBMysql
	self.m_connParam = {ip, user, pwd, db, port}
	local result = m_pDBMysql:connectMySql(ip, user, pwd, db, port)
	if not result then
		log_error("connect mysql failed", ip, user, pwd, db, port)
	end
	return result
end
function mysql:query(sql, skipResult)
    if skipResult then
        return self:queryHash(sql)
    else
        return self:executeQuery({sql}, skipResult)
    end
end
function mysql:queryHash(sql)
	local m_hash_queue = self.m_hash_queue
	m_hash_queue:push(sql)
	if m_hash_queue:count() > 100 then
		self:checkToSave()
	end
	return true
end
function mysql:getLastAutoID()
	return self.m_pDBMysql:getLastAutoID()
end
-- 更新一条数据
function mysql:update(table_name, id_name, data, skipResult)
    local sql = self:getUpdateSql(table_name, id_name, data)
    if skipResult then
        return self:queryHash(sql)
	end
	return self:executeQuery({sql}, skipResult)
end
-- 更新多条数据
function mysql:mupdate(tableArr, skipResult)
    if skipResult then
		for _,info in ipairs(tableArr) do
			local table_name = info.table_name
			local id_name = info.id_name
			local dataArr = info.data_arr
			for _,data in ipairs(dataArr) do
				local sql = self:getUpdateSql(table_name, id_name, data)
				self:queryHash(sql)
			end
		end
        return true
    end
    local sqlArr = {}
	for _,info in ipairs(tableArr) do
		local table_name = info.table_name
		local id_name = info.id_name
		local dataArr = info.data_arr
		for _,data in ipairs(dataArr) do
			local sql = self:getUpdateSql(table_name, id_name, data)
			table.insert(sqlArr, sql)
		end
	end
	return self:executeQuery(sqlArr, skipResult)
end
-- 插入一条数据
function mysql:insert(table_name, data, skipResult)
    local sql = self:getInsertSql(table_name, data)
    if skipResult then
        return self:queryHash(sql)
	end
	return self:executeQuery({sql}, skipResult)
end
-- 插入多条数据
function mysql:minsert(tableArr, skipResult)
    if skipResult then
		for _,info in ipairs(tableArr) do
			local table_name = info.table_name
--			local id_name = info.id_name
			local dataArr = info.data_arr
			for _,data in ipairs(dataArr) do
				local sql = self:getInsertSql(table_name, data)
				self:queryHash(sql)
			end
		end
        return true
	end
	local sqlArr = {}
	for _,info in ipairs(tableArr) do
		local table_name = info.table_name
--			local id_name = info.id_name
		local dataArr = info.data_arr
		for _,data in ipairs(dataArr) do
			local sql = self:getInsertSql(table_name, data)
			table.insert(sqlArr, sql)
		end
	end
	return self:executeQuery(sqlArr, skipResult)
end
-- 查询一条数据
function mysql:select(table_name, keyName, keyValue, columnNames)
    local sql = self:getSelectSql(table_name, keyName, keyValue, columnNames)
	return self:executeQuery({sql}, false)
end
-- 查询多条数据
function mysql:mselect(queryArr)
    local sqlArr = {}
    for _,query in ipairs(queryArr) do
        local table_name = query.table_name
        local keyName = query.key_name
        local keyValue = query.key_value
        local columnNames = query.column_names
        local sql = self:getSelectSql(table_name, keyName, keyValue, columnNames)
        table.insert(sqlArr, sql)
    end
	return self:executeQuery(sqlArr, false)
end
-- 删除一条数据
function mysql:delete(table_name, keyName, keyValue, skipResult)
    local sql = self:getDeleteSql(table_name, keyName, keyValue)
	return self:executeQuery({sql}, skipResult)
end
-- 删除多条数据
function mysql:mdelete(tableArr, skipResult)
    if skipResult then
		for _,info in ipairs(tableArr) do
			local table_name = info.table_name
			local keyName = info.key_name
			local valueArr = info.value_arr
			for _,keyValue in ipairs(valueArr) do
				local sql = self:getDeleteSql(table_name, keyName, keyValue)
				self:queryHash(sql)
			end
		end
        return true
    end
    local sqlArr = {}
	for _,info in ipairs(tableArr) do
		local table_name = info.table_name
		local keyName = info.key_name
		local valueArr = info.value_arr
		for _,keyValue in ipairs(valueArr) do
			local sql = self:getDeleteSql(table_name, keyName, keyValue)
			table.insert(sqlArr, sql)
		end
	end
	return self:executeQuery(sqlArr, skipResult)
end
-- 获取查询语句
function mysql:getUpdateSql(table_name, id_name, data)
	local m_pDBMysql = self.m_pDBMysql
    local id = data[id_name]
    local kvArr = {}
    local kv
    for k,v in pairs(data) do
		local v_type = type(v)
		if v_type == "string" then
			v = m_pDBMysql:escape(v) or v     -- 需要确保字符串编码的安全
			v = "\""..v.."\""
		end
		kv = k .. "=" .. v
		table.insert(kvArr, kv)
    end
    local sql
    if type(id) == "number" then
        sql = "update %s set %s where %s=%s;"
    else
        sql = "update %s set %s where %s=\"%s\";"
    end
    local valuestr = table.concat(kvArr, ",")
    sql = string.format( sql, table_name, valuestr, id_name, id )
    return sql
end
function mysql:getInsertSql(table_name, data)
	local m_pDBMysql = self.m_pDBMysql
    local keys = {}
    local values = {}
    for k,v in pairs(data) do
		local v_type = type(v)
		table.insert(keys, k)
		if v_type == "string" then
			v = m_pDBMysql:escape(v) or v    -- 需要确保字符串编码的安全
			v = "\""..v.."\""
		end
		table.insert(values, v)
    end
    local sql = "insert into %s (%s) values (%s);";
    local keystr = table.concat(keys, ",")
    local valuestr = table.concat(values, ",")
    sql = string.format( sql, table_name, keystr, valuestr )
    return sql
end
function mysql:getSelectSql(table_name, keyName, keyValue, columnNames)
	local m_pDBMysql = self.m_pDBMysql
    local sql
    if columnNames == nil then
        sql = "select * from"
    else
        sql = "select " .. table.concat(columnNames, ",").. " from"
    end
    local key_value_type = type(keyValue)
    if key_value_type == "nil" then
        sql = string.format("%s %s;", sql, table_name)
    elseif key_value_type == "number" then
        sql = string.format("%s %s where %s=%s;", sql, table_name, keyName, keyValue)
    else
        keyValue = m_pDBMysql:escape(keyValue) or keyValue    -- 需要确保字符串编码的安全
        sql = string.format("%s %s where %s=\"%s\";", sql, table_name, keyName, keyValue)
    end
    return sql
end
function mysql:getDeleteSql(table_name, keyName, keyValue)
	local m_pDBMysql = self.m_pDBMysql
    local sql
    if type(keyValue) == "number" then
        sql = "delete from %s where %s=%s;";
    else
        keyValue = m_pDBMysql:escape(keyValue) or keyValue    -- 需要确保字符串编码的安全
        sql = "delete from %s where %s=\"%s\";";
    end
    sql = string.format( sql, table_name, keyName, keyValue )
    return sql
end
function mysql:executeQuery(sqlArr, skipResult)
	local queryNumber = #sqlArr
	if queryNumber == 0 then
		log_error("queryNumber == 0")
		return true
	end
	skipResult = skipResult or false
	local query = table.concat(sqlArr)
	log_debug("skipResult", skipResult, "queryNumber", queryNumber, query)
	return self.m_pDBMysql:execute(query, queryNumber, skipResult)
end

return mysql

