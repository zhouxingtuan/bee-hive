--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 16/11/26
-- Time: 下午10:51
-- To change this template use File | Settings | File Templates.
--

local type = type
local pairs = pairs
local ipairs = ipairs
local os = os
local tonumber = tonumber
local string_find = string.find
local string_sub = string.sub
local string_len = string.len
local string_reverse = string.reverse
local table_insert = table.insert


local util = {}

local shuffle = function(tb)
	if not tb then return end
	local cnt = #tb
	for i=1,cnt do
		local j = math.random(i,cnt)
		tb[i],tb[j] = tb[j],tb[i]
	end
end
util.shuffle = shuffle

local copy = function(res)
	local lookup_table = {}
	local function _copy(object)
		if type(object) ~= "table" then
			return object
		elseif lookup_table[object] then
			return lookup_table[object]
		end
		local new_table = {}
		lookup_table[object] = new_table
		for key, value in pairs(object) do
			new_table[_copy(key)] = _copy(value)
		end
		return new_table
	end
	return _copy(res)
end
util.copy = copy

local split = function(value, mark)
	local arr = {}
	local fromn = 1
	local token
	local startn, endn = string_find(value, mark, fromn)
	while startn ~= nil do
		token = string_sub( value, fromn, startn-1 )
		table_insert(arr, token)
		fromn = endn + 1
		startn, endn = string_find(value, mark, fromn)
	end
	token = string_sub( value, fromn, #value )
	table_insert(arr, token)
	return arr
end
util.split = split
local count = function(tab)
	local c = 0
	for _,_ in pairs(tab) do
		c = c + 1
	end
	return c
end
util.count = count
local isEmpty = function(tab)
	if tab == nil or type(tab) ~= "table" then
		return true
	end

	for _,_ in pairs(tab) do
		return false
	end
	return true
end
util.isEmpty = isEmpty

-- 该方法可以将中间有nil值的参数完全解开
local function param_unpack(t, s)
	s = s or 1
	local max = 0
	for i,_ in pairs(t) do
		if i > max then
			max = i
		end
	end
	local function up(t, i)
		if i < max then
			return t[i],up(t,i+1)
		else
			return t[i]
		end
	end
	return up(t,s)
end
util.unpack = param_unpack

local merge = function(des, res)
	local lookup_table = {}
	local _merge, _mergeTable
	_mergeTable = function(des_table, res_table)
		for k, v in pairs(res_table) do
			if type(k) == "table" then
				_merge(des_table, copy(k), v)
			else
				_merge(des_table, k, v)
			end
		end
	end
	_merge = function(dest, key, value)
		if type(value) ~= "table" then
			dest[key] = value
			return
		elseif lookup_table[value] then
			dest[key] = lookup_table[value]
			return
		end
		local dest_table = dest[key]
		if dest_table == nil or type(dest_table) ~= "table" then
			dest_table = {}
			dest[key] = dest_table
		end
		lookup_table[value] = dest_table
		_mergeTable(dest_table, value)
	end
	_mergeTable(des, res)
end
util.merge = merge

local checkType = function(obj, objtype)
	if type(objtype) ~= "string" then
		return false
	end

	if type(obj) ~= objtype then
		return false
	end

	return true
end

util.checkType = checkType

local getFileNameOrPath = function( strpath, bname)  
    local ts = string_reverse(strpath)  
    local _, param2 = string_find(ts, "/")  -- 这里以"/"为例  
	local len = string_len(strpath) 
    local m = len - param2 + 1     
    local result  
    if bname then  
        result = string_sub(strpath, m+1, len)   
    else  
        result = string_sub(strpath, 1, m-1)   
    end  
  
    return result  
end  
util.getFileNameOrPath = getFileNameOrPath

local existArrayValue = function (tb, value)
	if not checkType(tb, "table") then
		return
	end

	for i,v in ipairs(tb) do
		if v == value then
			return true, i
		end
	end
end
util.existArrayValue = existArrayValue

local existHashValue = function (tb, value)
	if not checkType(tb, "table") then
		return
	end

	for k,v in pairs(tb) do
		if v == value then
			return true, k
		end
	end
end
util.existHashValue = existHashValue

local function get_timestamp(start_date, start_time)
	local date_arr = util.split(start_date, "-")
	local time_arr = util.split(start_time, ":")
	local year = tonumber(date_arr[1])
	local month = tonumber(date_arr[2])
	local day = tonumber(date_arr[3])
	local hour = tonumber(time_arr[1])
	local min = tonumber(time_arr[2])
	local sec = tonumber(time_arr[3])
	local new_t = { year=year, month=month, day=day, hour=hour, min=min, sec=sec }
	local active_time = os.time(new_t)
	local today_second = hour*3600 + min*60 + sec
	return active_time,today_second
end
util.get_timestamp = get_timestamp
local function get_today_seconds(start_time)
	local time_arr = util.split(start_time, ":")
	local hour = tonumber(time_arr[1])
	local min = tonumber(time_arr[2])
	local sec = tonumber(time_arr[3])
	local today_second = hour*3600 + min*60 + sec
	return today_second
end
util.get_today_seconds = get_today_seconds
local php_week_to_lua_wday = {
	[1] = 2;
	[2] = 3;
	[3] = 4;
	[4] = 5;
	[5] = 6;
	[6] = 7;
	[7] = 1;
}
util.php_week_to_lua_wday = php_week_to_lua_wday

return util
