--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/5/25
-- Time: 下午10:00
-- To change this template use File | Settings | File Templates.
--

local coroutine = coroutine
local pairs = pairs
local table = table
local string = string
local ipairs = ipairs
local type = type
local log_debug = log_debug
local log_error = log_error
local log_info = log_info

local function outputMsgID(msgID)
	return string.format("0x%x", msgID)
end

-- COMMAND_CLIENT_REQUEST = 1
-- COMMAND_HTTP_REQUEST = 2
-- COMMAND_NODE_REQUEST = 3
-- COMMAND_NODE_RESPONSE = 4
-- COMMAND_NODE_ERROR = 5
-- COMMAND_REQUEST_TIMEOUT = 6
-- COMMAND_BROADCAST = 7

local class = require("class")
local protobuf = require("protobuf")
local rapidjson = require("rapidjson")
local server_type = require("server_type")

local rpc = class("rpc")

-- C++调用函数 Begin-------------------

function rpc:onInitialize(pNode, param)
	log_debug("rpc:onInitialize called", pNode:getNodeID(), param)
	return self:initRpc(pNode, param)
end
function rpc:onDestroy()
	log_debug("rpc:onDestroy called")
	return self:destroyRpc()
end
function rpc:onReceiveAccept(pAccept, buffer, command, callbackID, desType, desIndex, resType, resIndex, message, uid)
	local connType = pAccept:getType()
	local connHandle = pAccept:getHandle()
	local rpcReq = {
		pConnect = pAccept;
		connType = connType;
		connHandle = connHandle;
		msgType = command;
		callbackID = callbackID;
		desType = desType;
		desIndex = desIndex;
		resType = resType;
		resIndex = resIndex;
		msgID = message;
		uid = uid;
	}
	log_debug("onReceiveAccept", connType, connHandle, command, callbackID, desType, desIndex, resType, resIndex, outputMsgID(message), uid)
	return self:handleMessage(buffer, rpcReq)
end
function rpc:onCloseAccept(pAccept)
	local connType = pAccept:getType()
	local connHandle = pAccept:getHandle()
	log_debug("onCloseAccept", connType, connHandle)
	return self:closeAcceptCall(pAccept, connType, connHandle)
end
function rpc:onReceiveHttp(pHttp, buffer, command, callbackID, desType, desIndex, resType, resIndex, message, uid)
	local connType = pHttp:getType()
	local connHandle = pHttp:getHandle()
	local rpcReq = {
		pConnect = pHttp;
		connType = connType;
		connHandle = connHandle;
		msgType = command;
		callbackID = callbackID;
		desType = desType;
		desIndex = desIndex;
		resType = resType;
		resIndex = resIndex;
		msgID = message;
		uid = uid;
	}
	log_debug("onReceiveHttp", connType, connHandle, command, callbackID, desType, desIndex, resType, resIndex, outputMsgID(message))
	return self:handleMessage(buffer, rpcReq)
end
function rpc:onCloseHttp(pHttp)
	log_debug("onCloseHttp", pHttp:getType(), pHttp:getHandle())
end
function rpc:onCurlResponse(isRequestOK, callbackID, buffer)
	log_debug("rpc:onCurlResponse called", isRequestOK, callbackID, #buffer)
	return self:doingHttp(isRequestOK, callbackID, buffer)
end
function rpc:onTimerUpdate(callbackID)
	return self:doingTimeout(callbackID)
end

-- C++调用函数 End-------------------

-- 对外接口
-- 通过uid获取玩家对象所在svrId
function rpc:getUserServerID(uid)
	return uid % 100 + 1
end
-- 通知某个连接
function rpc:notifyConnect(msgID, connType, connHandle, request)
    local buffer
    if type(request) == "string" then
        buffer = request
    else
        local info = self.m_msgMap[msgID]
        buffer = self:encodeRequest(info, request)
    end
	if not buffer then
		log_error("rpc:notifyConnect encodeRequest failed", outputMsgID(msgID), request)
		return false
	end
	local resType = 0
	local resIndex = 0
	local callbackID = 0
	local uid = 0
    log_debug("notifyConnect", buffer)
	local result = self.m_pNode:responseConnect(resType, resIndex, msgID, callbackID, uid, buffer, #buffer, connType, connHandle)
	if not result then
		log_error("rpc:notifyConnect for response failed", outputMsgID(msgID), callbackID, resType, resIndex)
	else
		log_debug("rpc:notifyConnect for response OK", outputMsgID(msgID), callbackID, resIndex)
	end
	return result
end
-- 主动推消息给客户端
function rpc:pushUser(uid, msgID, request)
	if uid >= 100000 and uid < 1000000 then
		return
	end
	log_debug("pushUser", uid, outputMsgID(msgID))
	local desID = self:getUserServerID(uid)
	local buffer
	if type(request) == "string" then
		buffer = request
	else
		buffer = self:encodeRequest(msgID, request)
	end
	if not buffer then
		return false
	end
	local msg = {
		cmd = msgID;
		buffer = buffer;
	}
	if self:getNodeType() == server_type.user and self:getNodeID() == desID then
		local res = {}
		local rpcReq = {}
		rpcReq.uid = uid
		rpcReq.resType = server_type.user
		rpcReq.resIndex = desID
		log_debug("pushUser from current....", uid)
		return self:rpcPush(msg, res, rpcReq)
	end
	local desType = server_type.user
	local response
	local timeout = 10000
	return self:request(desType, desID, 0x98, uid, msg, response, timeout)
end
-- 广播数据给所有在线玩家
function rpc:broadcastOnline(request)
    local desType = server_type.access
    local msgID = 0x3030
    return self:broadcast(desType, msgID, 0, request)
end
-- 广播给某种类型的服务
function rpc:broadcast(desType, msgID, uid, request)
	log_debug("broadcast", desType, uid, outputMsgID(msgID))
	uid = uid or 0
	local buffer = self:encodeRequest(msgID, request)
	if not buffer then
		return false
	end
	local result = self.m_pNode:broadcast(desType, msgID, uid, buffer, #buffer)
	if not result then
		log_error("rpc:broadcast failed", desType, outputMsgID(msgID), request)
		return false
	end
	return true
end
-- 请求某个服务
function rpc:request(desType, desID, msgID, uid, request, response, timeout)
	timeout = timeout or 10000
	uid = uid or 0
	log_debug("request", desType, desID, uid, outputMsgID(msgID))
	local callbackID = 0
	if response ~= nil then
		local co = coroutine.running()
		if co ~= nil then
			local coInfo = self:getInfoByCO(co)
			if coInfo then
				callbackID = coInfo.callbackID
			else
				log_error("rpc:request but no co info found", outputMsgID(msgID))
			end
		else
			log_error("rpc:request but no running co found", outputMsgID(msgID))
		end
	end
	local buffer = self:encodeRequest(msgID, request)
	if not buffer then
		log_error("encodeRequest failed")
		return false
	end
	local result = self.m_pNode:sendRequest(desType, desID, msgID, callbackID, uid, buffer, #buffer);
	if not result then
		log_error("rpc:request for sendRequest failed", desType, desID, outputMsgID(msgID), callbackID, request)
		return false
	end
	if response ~= nil then
		local isOK,responseValue = coroutine.yield(timeout)
		if not isOK then
			log_error("rpc:request for timeout", desID, desType, outputMsgID(msgID), callbackID, responseValue)
			return false
		end
		for k,v in pairs(responseValue) do
			response[k] = v
		end
	end
	return true
end
-- 发起一个curl的http请求
function rpc:requestHttp(method, url, headers, body, response, timeout)
	method = method:upper()		-- C++ 只认大写
	local co = coroutine.running()
	--	log_debug("requestHttp current running coroutine", co)
	if co == nil then
		log_error("requestHttp should have a coroutine running")
		return false
	end
	local coInfo = self:getInfoByCO(co)
	if coInfo == nil then
		log_error("requestHttp can not find coroutine info")
		return false
	end
	local callbackID = coInfo.callbackID
	if method == "POST" and headers["Content-Length"] == nil then
		headers["Content-Length"] = #body
	end
	local result = self.m_pNode:sendCurlRequest(method, url, headers, body, callbackID)
	if not result then
		log_error("rpc:requestHttp for sendCurlRequest failed", result, method, url, headers, callbackID)
		return false
	end
	local isOK,responseValue = coroutine.yield(timeout)
	if not isOK then
		log_error("rpc:requestHttp for timeout", method, url, headers, callbackID)
		return false
	end
	for k,v in pairs(responseValue) do
		response[k] = v
	end
	return true
end
-- 注册一个消息的处理函数和协议
function rpc:register(msgID, obj, objFunc)
	local msgInfo = self.m_msgMap[msgID]
	if msgInfo == nil then
		log_error("rpc:register failed msgInfo == nil", outputMsgID(msgID))
		return
	end
	log_info("register", outputMsgID(msgID))
	msgInfo.obj = obj
	msgInfo.objFunc = objFunc
end
-- 执行一个携程函数
function rpc:execute(callback)
	-- 创建携程
	local co = coroutine.create(callback)
	-- 注册当前的携程
	local callbackID,coInfo = self:registerCoroutine(co)
	if not self:resumeCoroutine(callbackID, coInfo, coInfo) then
		log_error("rpc:execute checkCoroutine failed")
		return false
	end
	return true,callbackID,coInfo
end
-- 设置一个计时器，单位毫秒
function rpc:schedule(callback, timeout, loop)
	local coCallback = function(coInfo)
		self:scheduleTimeout(coInfo, timeout)
--		log_debug("rpc:schedule begin", coInfo.callbackID, coInfo.timerHandle, timeout)
		coroutine.yield(timeout)
--		log_debug("rpc:schedule callback begin", coInfo.callbackID, coInfo.timerHandle, timeout)
		if loop then
			while true do
--				log_debug("rpc:schedule callback loop", coInfo.callbackID, coInfo.timerHandle, timeout)
				callback(coInfo)
				self:changeCoroutineCallbackID(coInfo)
				coroutine.yield(timeout)
			end
		else
			callback(coInfo)
		end
--		log_debug("rpc:schedule callback end", coInfo.callbackID, coInfo.timerHandle, timeout)
	end
	return self:execute(coCallback)
end
function rpc:addListener(obj)
	local m_listener = self.m_listener
	if m_listener  == nil then
		m_listener = {}
		self.m_listener = m_listener
	end
	for _,oldObj in ipairs(m_listener) do
		if oldObj == obj then
			return
		end
	end
	log_info("addListener", obj.__cname)
	table.insert(m_listener, obj)
end
-- 发送消息request，不挂起
function rpc:sendRequest(desType, desID, msgID, uid, buffer)
	uid = uid or 0
	local callbackID = 0
	local co = coroutine.running()
	if co ~= nil then
		local coInfo = self:getInfoByCO(co)
		if coInfo then
			callbackID = coInfo.callbackID
		else
			log_error("rpc:request but no co info found", outputMsgID(msgID))
		end
	else
		log_error("rpc:request but no running co found", outputMsgID(msgID))
	end
	local result = self.m_pNode:sendRequest(desType, desID, msgID, callbackID, uid, buffer, #buffer);
	if not result then
		log_error("rpc:request for sendRequest failed", desType, desID, outputMsgID(msgID), callbackID)
		return false
	end
	return true,callbackID
end
-- 动态注册cmd
function rpc:registerRpcProto(msgID, msgSource, requestName, responseName)
	local m_msgMap = self.m_msgMap
	local msgInfo = m_msgMap[msgID]
	log_info("rpcproto", outputMsgID(msgID), msgSource, requestName, responseName)
	if msgInfo == nil then
		msgInfo = { msgID=msgID, msgSource=msgSource, requestName=requestName, responseName=responseName }
		m_msgMap[msgID] = msgInfo
	else
		msgInfo.msgID = msgID
		msgInfo.msgSource = msgSource
		msgInfo.requestName = requestName
		msgInfo.responseName = responseName
	end
end


-- 内部接口 -------------------------------------------
-- 目前只有UserSer需要处理这个协议
function rpc:rpcPush(request, response, rpcReq)
	local curType = self:getNodeType()
	local uid = rpcReq.uid
	if curType == server_type.user then
		-- 查询玩家的accessId，往access发送消息
		log_debug("rpcPush to access", outputMsgID(request.cmd), uid)
		local desID = self:accessCall(uid)
		if desID == nil then
			log_error("rpcPush can not find access ID", outputMsgID(request.cmd), uid)
			return
		end
		local desType = server_type.access
		local my_response
		local timeout = 100000
		return self:request(desType, desID, 0x98, uid, request, my_response, timeout)
	elseif curType == server_type.access then
		-- 查询玩家的连接信息，往客户端发消息
		log_debug("rpcPush to client", outputMsgID(request.cmd), uid)
		local connHandle = self:clientCall(uid)
		if connHandle == nil then
			log_error("rpcPush can not find client ID", outputMsgID(request.cmd), uid)
			return
		end
		local resType = rpcReq.resType
		local resIndex = rpcReq.resIndex
		local msgID = request.cmd
		local callbackID = 0
		local responseValue = request.buffer
		local connType = 1	-- CONN_TYPE_ACCEPT = 1
		local result = self.m_pNode:responseConnect(resType, resIndex, msgID, callbackID, uid, responseValue, #responseValue, connType, connHandle)
		if not result then
			log_error("rpc:doingRequest for response failed", outputMsgID(msgID), callbackID, resType, resIndex, connType, connHandle)
		else
			log_debug("rpc:doingRequest for response OK", outputMsgID(msgID), callbackID, resIndex)
		end
	else
		-- 往user发送消息
		log_debug("rpcPush why transfer again ?", outputMsgID(request.cmd), uid)
		return self:pushUser(uid, request.cmd, request.buffer)
	end
end
-- 热更新，每个服务都会注册该消息协议
function rpc:hotUpdate(request, response, rpcReq)
	log_info("hotUpdate called", request)
	package.loaded["hotupdate_start"] = nil
	require("hotupdate_start")

	package.loaded["protobuf"] = nil
	protobuf = require("protobuf")

	local config = require("config")
	local p = "script."..config.moduleName..".init"
	log_info("hotUpdate reload", p)
	package.loaded[p] = nil
	require(p)
	self:initRpcProto()
	self:registerCall()
	package.loaded["hotupdate_end"] = nil
	require("hotupdate_end")
	response.result = true
end

function rpc:handleMessage(buffer, rpcReq)
	local msgType = rpcReq.msgType
	local msgID = rpcReq.msgID
	local callbackID = rpcReq.callbackID
	local coInfo
	if msgType == 4 then
		coInfo = self:getInfoByID(callbackID)
		if coInfo == nil then
			log_error("rpc:handleMessage can not find coInfo", callbackID)
			return
		end
		if coInfo.isTransfer then
			log_debug("handleTransfer coInfo.isTransfer true")
			return self:handleTransfer(msgType, msgID, buffer, rpcReq)
		end
	else
		local info = self.m_msgMap[msgID]
		-- 找不到当前注册cmd的处理函数
		if msgType ~= 5 and ( info == nil or info.obj == nil )  then
			log_debug("handleTransfer info == nil or info.obj == nil", type(info))
			return self:handleTransfer(msgType, msgID, buffer, rpcReq)
		end
	end
	-- 解析协议
	local data
	data = self:decodeRpc(msgType, msgID, buffer)
	if not data then
		log_error("handleMessage decodeRequest failed", rpcReq)
		return
	end
	return self:doing(data, rpcReq)
end
function rpc:handleTransfer(msgType, msgID, buffer, rpcReq)
	-- 如果是客户端协议，并且找不到本地处理的cmd，那么直接转发这个消息到对应的user服务器
	-- COMMAND_NODE_RESPONSE = 4
	if msgType == 4 then
		return self:response(buffer, rpcReq)
	end
	-- 如果是access的协议，并且找不到本地处理的cmd，那么直接转发消息到对应的服务器
	local resType = rpcReq.resType
	-- COMMAND_CLIENT_REQUEST = 1
	if msgType == 1 or resType == server_type.access then
		-- 创建一个携程处理当前的请求
		local callback = function()
			local desType
			local desID
			local uid = rpcReq.uid
			if msgType == 1 then
				desType = server_type.user
				local recordUID = self:uidCall(rpcReq.connType, rpcReq.connHandle)
                if uid ~= recordUID then
					log_debug("uid to connect is not equal", uid, recordUID)
                    uid = recordUID or 0
                end
				desID = self:getUserServerID(uid)
				log_debug("send to user from access", uid, desType, desID, outputMsgID(msgID))
			else
				desType,desID = self:routeCall(uid, msgID)
				log_debug("send to other from user", uid, desType, desID, outputMsgID(msgID))
				if desType == nil then
					log_error("user route is nil", uid, outputMsgID(msgID))
					return
				end
			end
			-- 检查是否需要返回消息给对方
			local callbackID = rpcReq.callbackID
			if callbackID > 0 then
				local result,newCallbackID = self:sendRequest(desType, desID, msgID, uid, buffer)
				if not result then
					log_error("sendRequest failed", desType, desID, outputMsgID(msgID), uid)
					return
				end
				local coInfo = self:getInfoByID(newCallbackID)
				coInfo.isTransfer = true
				local isOK,responseValue = coroutine.yield(10000)
				if not isOK then
					log_error("rpc:handleTransfer for timeout", newCallbackID)
					return
				end
				local resType = rpcReq.resType
				local resIndex = rpcReq.resIndex
				log_debug("uid", uid, outputMsgID(msgID), "responseConnect buffer length", #responseValue)
				local result = self.m_pNode:responseConnect(resType, resIndex, msgID, callbackID, uid, responseValue, #responseValue, rpcReq.connType, rpcReq.connHandle)
				if not result then
					log_error("rpc:doingRequest for response failed", outputMsgID(msgID), callbackID, resType, resIndex)
				else
					log_debug("rpc:doingRequest for response OK", outputMsgID(msgID), callbackID, resIndex)
				end
			end
		end
		if not self:execute(callback) then
			log_error("rpc:doingRequest execute error", outputMsgID(msgID))
			return
		end
		log_debug("rpc:doingRequest execute OK", outputMsgID(msgID))
		return
	end
	log_error("can not transfer message", msgType, outputMsgID(msgID), rpcReq.uid, resType)
end
function rpc:initRpc(pNode, param)
	self.m_pNode = pNode
	self.m_nodeType = pNode:getModuleType()
	self.m_nodeID = pNode:getNodeID()
	self.m_param = param
	self.m_msgMap = {}
	self.m_cbMap = {}
	self.m_coMap = {}
	self.m_id = 1	-- (1,2147483647)

	-- 初始化config的配置，建立服务器组间的连接
	if pNode ~= nil then
		-- Handler 是由主线程创建的，初始化操作需要等到后面执行，这里使用Timer延迟执行
		self:schedule(function()
			return self:delayInit()
		end, 1000)
	end
end
function rpc:getNode()
	return self.m_pNode
end
function rpc:getNodeType()
	return self.m_nodeType
end
function rpc:getNodeID()
	return self.m_nodeID
end
function rpc:delayInit()
	self:initRpcProto()
	self:initCall()
	self:registerCall()
	-- rpc 监听热更新命令和接口
	rpc:register(0x98, self, self.rpcPush)
	rpc:register(0x99, self, self.hotUpdate)
end
function rpc:initCall()
	local pNode = self.m_pNode
	local param = self.m_param
	local m_listener = self.m_listener
	if m_listener then
		for _,obj in ipairs(m_listener) do
			local onInitialize = obj.onInitialize
			if onInitialize then
				onInitialize(obj, pNode, param)
			else
				log_error("Object do not provide onInitialize function", obj.__cname)
			end
		end
	else
		log_error("initCall m_listener == nil")
	end
end
function rpc:registerCall()
	local m_listener = self.m_listener
	if m_listener then
		for _,obj in ipairs(m_listener) do
			local onRegister = obj.onRegister
			if onRegister then
				onRegister(obj)
			else
				log_error("Object do not provide onRegister function", obj.__cname)
			end
		end
	else
		log_error("registerCall m_listener == nil")
	end
end
function rpc:closeAcceptCall(pAccept, connType, connHandle)
	local m_listener = self.m_listener
	if m_listener then
		for _,obj in ipairs(m_listener) do
			local onCloseConnect = obj.onCloseConnect
			if onCloseConnect then
				onCloseConnect(obj, pAccept, connType, connHandle)
			else
				log_error("Object do not provide onCloseConnect function", obj.__cname)
			end
		end
	else
		log_error("registerCall m_listener == nil")
	end
end
function rpc:routeCall(uid, msgID)
	local m_listener = self.m_listener
	if m_listener then
		for _,obj in ipairs(m_listener) do
			local onRouteRequest = obj.onRouteRequest
			if onRouteRequest then
				local desType,desID = onRouteRequest(obj, uid, msgID)
				if desType and desID then
					return desType,desID
				end
			end
		end
	else
		log_error("registerCall m_listener == nil")
	end
end
function rpc:uidCall(connType, connHandle)
	local m_listener = self.m_listener
	if m_listener then
		for _,obj in ipairs(m_listener) do
			local onQueryUID = obj.onQueryUID
			if onQueryUID then
				local uid = onQueryUID(obj, connType, connHandle)
				if uid then
					return uid
				end
			end
		end
	else
		log_error("registerCall m_listener == nil")
	end
end
function rpc:accessCall(uid)
	local m_listener = self.m_listener
	if m_listener then
		for _,obj in ipairs(m_listener) do
			local onQueryAccessID = obj.onQueryAccessID
			if onQueryAccessID then
				local desID = onQueryAccessID(obj, uid)
				if desID then
					return desID
				end
			end
		end
	else
		log_error("registerCall m_listener == nil")
	end
end
function rpc:clientCall(uid)
	local m_listener = self.m_listener
	if m_listener then
		for _,obj in ipairs(m_listener) do
			local onQueryClientID = obj.onQueryClientID
			if onQueryClientID then
				local desID = onQueryClientID(obj, uid)
				if desID then
					return desID
				end
			end
		end
	else
		log_error("registerCall m_listener == nil")
	end
end
function rpc:initRpcProto()
	package.loaded["rpcproto"] = nil
	package.loaded["script.pb.init"] = nil
	require("script.pb.init")
	local rpcproto = require("rpcproto")
	local m_msgMap = self.m_msgMap
	for _,info in ipairs(rpcproto) do
		local msgID = info[1]
		local msgSource = info[2]
		local requestName = info[3]
		local responseName = info[4]
		local msgInfo = m_msgMap[msgID]
		log_info("rpcproto", outputMsgID(msgID), msgSource, requestName, responseName)
		if msgInfo == nil then
			msgInfo = { msgID=msgID, msgSource=msgSource, requestName=requestName, responseName=responseName }
			m_msgMap[msgID] = msgInfo
		else
			msgInfo.msgID = msgID
			msgInfo.msgSource = msgSource
			msgInfo.requestName = requestName
			msgInfo.responseName = responseName
		end
	end
end
function rpc:destroyRpc()
	log_debug("rpc:destroyRpc called")
	local pNode = self.m_pNode
	for _,coInfo in pairs(self.m_cbMap) do
		local timerHandle = coInfo.timerHandle
		if timerHandle then
			pNode:removeTimer(timerHandle)
		end
	end
	local m_listener = self.m_listener
	if m_listener then
		for _,obj in ipairs(m_listener) do
			local onDestroy = obj.onDestroy
			if onDestroy then
				onDestroy(obj)
			else
				log_error("Object do not provide onDestroy function", obj.__cname)
			end
		end
	else
		log_error("destroyRpc m_listener == nil")
	end
	self:initRpc(nil)
end
function rpc:registerCoroutine(co)
	local callbackID = self:nextID()
	local coInfo = { callbackID=callbackID, co=co }
	self.m_cbMap[callbackID] = coInfo
	self.m_coMap[co] = coInfo
	return callbackID,coInfo
end
function rpc:changeCoroutineCallbackID(coInfo)
	-- 注释掉老的callbackID
	local callbackID = coInfo.callbackID
	self.m_cbMap[callbackID] = nil
	-- 生成新的callbackID
	callbackID = self:nextID()
	coInfo.callbackID = callbackID
	self.m_cbMap[callbackID] = coInfo
	return callbackID,coInfo
end
function rpc:getInfoByID(callbackID)
	return self.m_cbMap[callbackID]
end
function rpc:getInfoByCO(co)
	return self.m_coMap[co]
end
function rpc:getTimerLeft(coInfo)
	local timerHandle = coInfo.timerHandle
	if timerHandle ~= nil then
		return self.m_pNode:getTimerLeft(timerHandle)
	end
	return 0
end
function rpc:remove(coInfo)
	return self:removeByID(coInfo.callbackID)
end
function rpc:removeByID(callbackID)
	log_debug("rpc:removeByID", callbackID)
	local coInfo = self.m_cbMap[callbackID]
	if coInfo ~= nil then
		self.m_cbMap[callbackID] = nil
		self.m_coMap[coInfo.co] = nil
		local timerHandle = coInfo.timerHandle
		log_debug("rpc:removeByID", timerHandle, callbackID)
		if timerHandle then
			log_debug("rpc:removeByID removeTimer", timerHandle, callbackID)
			self.m_pNode:removeTimer(timerHandle)
		end
	else
		log_error("rpc:removeByID not found coInfo", callbackID)
	end
end
function rpc:nextID()
	local id = self.m_id + 1
	if id > 2147483647 then
		self.m_id = 2
		id = 1
	else
		self.m_id = id
	end
	return id
end
function rpc:encodeRequest(msgID, request)
	local info = self.m_msgMap[msgID]
	if info == nil then
		log_error("encodeRequest failed info == nil", outputMsgID(msgID))
		return
	end
	if info.msgSource == "pb" then
		-- log_debug("requestName:", info.requestName, ", request:", request)
		local buffer = protobuf.encode(info.requestName, request)
		if not buffer then
			log_error("encodeRequest failed protobuf.encode", outputMsgID(msgID), info.requestName, request)
			return
		end
		return buffer
	else
		-- json
		local buffer = rapidjson.encode(request)
		return buffer
	end
end
function rpc:encodeResponse(info, response)
	if info.msgSource == "pb" then
		local buffer = protobuf.encode(info.responseName, response)
		if not buffer then
			log_error("encodeResponse failed protobuf.encode", outputMsgID(info.msgID))
			return
		end
		return buffer
	else
		-- json
		local buffer = rapidjson.encode(response)
		return buffer
	end
end
function rpc:decodeRpc(msgType, msgID, buffer)
	local info = self.m_msgMap[msgID]
	if info == nil then
		log_error("decodeRequest failed info == nil", outputMsgID(msgID))
		return
	end
	-- 特殊处理
	if buffer == "" then
		log_debug("buffer for rpc is empty", msgType, outputMsgID(msgID))
		return {}
	end
	if info.msgSource == "pb" then
		local pbName
		-- COMMAND_NODE_RESPONSE = 4
		if msgType == 4 then
			pbName = info.responseName
		else
			pbName = info.requestName
		end
--		local request = protobuf.decode(pbName, buffer)
		local request = protobuf.decodeAll(pbName, buffer)
		if not request then
			log_error("decodeRequest failed protobuf.decode", outputMsgID(msgID))
			return
		end
		return request
	else
		local request = rapidjson.decode(buffer)
		return request
	end
end
function rpc:doingHttp(isOK, callbackID, buffer)
	log_debug("rpc:doingHttp with callbackID", isOK, callbackID)
	local coInfo = self:getInfoByID(callbackID)
	if coInfo == nil then
		log_error("rpc:doingHttp can not find coInfo", callbackID)
		return
	end
	local co = coInfo.co
	-- 执行携程函数
	local response = {
		isOK = isOK;
		buffer = buffer;
	}
	if not self:resumeCoroutine(callbackID, coInfo, true, response) then
		log_error("rpc:doingHttp checkCoroutine failed")
		return
	end
end
function rpc:doing(request, rpcReq)
	local msgType = rpcReq.msgType
	-- COMMAND_CLIENT_REQUEST = 1
	-- COMMAND_HTTP_REQUEST = 2
	-- COMMAND_NODE_REQUEST = 3
	-- COMMAND_NODE_RESPONSE = 4
	-- COMMAND_NODE_ERROR = 5
	-- COMMAND_REQUEST_TIMEOUT = 6
	-- COMMAND_BROADCAST = 7
	if msgType < 4 then
		return self:doingRequest(request, rpcReq)
	elseif msgType == 4 then
		return self:response(request, rpcReq)
	elseif msgType == 5 then
		-- 处理消息路由失败
	elseif msgType == 7 then
		return self:doingRequest(request, rpcReq)
	else
		log_error("rpc:doing unknown msgType", msgType)
	end
end
function rpc:doingRequest(request, rpcReq)
	-- 查找消息的注册信息
	local msgID = rpcReq.msgID
	local info = self.m_msgMap[msgID]
	local obj = info.obj
	local objFunc = info.objFunc
	if obj == nil or objFunc == nil then
		log_error("rpc:doingRequest for obj == nil or objFunc == nil", msgID)
		return
	end
	-- 创建一个携程处理当前的请求
	local callback = function()
		local response = {}
		log_debug("rpc:doingRequest call obj function begin", outputMsgID(msgID))
		objFunc(obj, request, response, rpcReq)
		log_debug("rpc:doingRequest call obj function end", outputMsgID(msgID))
		-- 检查是否需要返回消息给对方
		local callbackID = rpcReq.callbackID
		-- http请求必须回复-- COMMAND_HTTP_REQUEST = 2
		if callbackID > 0 or rpcReq.msgType == 2 then
			local resType = rpcReq.resType
			local resIndex = rpcReq.resIndex
			local uid = rpcReq.uid
			local buffer = self:encodeResponse(info, response)
			if not buffer then
				log_error("rpc:doingRequest encodeResponse failed", outputMsgID(msgID), response)
				return
			end
			local result = self.m_pNode:responseConnect(resType, resIndex, msgID, callbackID, uid, buffer, #buffer, rpcReq.connType, rpcReq.connHandle)
			if not result then
				log_error("rpc:doingRequest for response failed", outputMsgID(msgID), callbackID, resType, resIndex)
			else
				log_debug("rpc:doingRequest for response OK", outputMsgID(msgID), callbackID, resIndex)
			end
		else
			log_debug("rpc:doingRequest for no response", outputMsgID(msgID), callbackID, rpcReq.resType, rpcReq.resIndex)
		end
	end
	if not self:execute(callback) then
		log_error("rpc:doingRequest execute error", outputMsgID(msgID))
		return
	end
	log_debug("rpc:doingRequest execute OK", outputMsgID(msgID))
end
function rpc:response(response, rpcReq)
	local callbackID = rpcReq.callbackID
	log_debug("rpc:response with callbackID", callbackID, outputMsgID(rpcReq.msgID))
	local coInfo = self:getInfoByID(callbackID)
	if coInfo == nil then
		log_error("rpc:response can not find coInfo", callbackID)
		return
	end
	local co = coInfo.co
	if not self:resumeCoroutine(callbackID, coInfo, true, response) then
		log_error("rpc:response checkCoroutine failed")
		return
	end
end
function rpc:scheduleTimeout(coInfo, timeout)
	local timerHandle = coInfo.timerHandle
	local callbackID = coInfo.callbackID
--	log_debug("rpc:scheduleTimeout", callbackID, timerHandle)
	if timerHandle ~= nil then
--		log_debug("scheduleTimeout changeTimer", timerHandle, callbackID)
		self.m_pNode:changeTimer(timerHandle, timeout)
	else
		timerHandle = self.m_pNode:startTimer(callbackID, timeout)
		coInfo.timerHandle = timerHandle
--		log_debug("scheduleTimeout startTimer", timerHandle, callbackID)
	end
end
function rpc:doingTimeout(callbackID)
--	log_debug("rpc:doingTimeout with callbackID", callbackID)
	local coInfo = self:getInfoByID(callbackID)
	if coInfo == nil then
		log_error("rpc:doingTimeout can not find coInfo", callbackID)
		return
	end
	-- 定时器超时的时候，不需要再执行后续的删除timer操作
	coInfo.timerHandle = nil
	if not self:resumeCoroutine(callbackID, coInfo, false) then
		log_error("rpc:doingTimeout checkCoroutine failed")
		return
	end
end
function rpc:processWithCO(functionName, ...)
	local co = coroutine.running()
--	log_debug("processWithCO current running coroutine", functionName, co)
	if co == nil then
		log_error("processWithCO should have a coroutine running", functionName)
		return false
	end
	local coInfo = self:getInfoByCO(co)
	if coInfo == nil then
		log_error("processWithCO can not find coroutine info", functionName)
		return false
	end
	local callbackID = coInfo.callbackID
	local pNode = self.m_pNode
	local func = pNode[functionName]
	log_debug("processWithCO before call", functionName)
	func(pNode, callbackID, ...)
	log_debug("processWithCO after call", functionName)
	local isOK,responseValue = coroutine.yield(10000)	-- 最多等待10秒返回结果
	log_debug("processWithCO after yield", functionName)
	return isOK,responseValue
end
function rpc:doingWithCO(callbackID, resultHandle)
	log_debug("rpc:doingWithCO with callbackID", callbackID, resultHandle)
	local coInfo = self:getInfoByID(callbackID)
	if coInfo == nil then
		log_error("rpc:doingWithCO can not find coInfo", callbackID)
		return
	end
	local co = coInfo.co
	local result = (resultHandle > 0)
	if not self:resumeCoroutine(callbackID, coInfo, result, resultHandle) then
		log_error("rpc:doingWithCO checkCoroutine failed")
		return
	end
end
function rpc:resumeCoroutine(callbackID, coInfo, ...)
	local co = coInfo.co
	-- 执行携程函数
	local result,returnValue = coroutine.resume(co, ...)
	if not result then
		log_error("rpc:checkCoroutine failed", returnValue)
		self:removeByID(callbackID)
		return false
	end
	-- 当前的请求被挂起了，需要处理挂起操作
	local co_status = coroutine.status(co)
--	log_debug("rpc:resumeCoroutine co_status", co_status, type(co_status), callbackID)
	if co_status == "suspended" then
		-- 设定超时移除的计时器 returnValue 为 timeout
		self:scheduleTimeout(coInfo, returnValue)
	else
		-- 执行结束，直接移除当前注册的携程
		self:removeByID(callbackID)
	end
	return true
end

return rpc
