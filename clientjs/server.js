
// 定义全局变量
// 导出模块函数
window.exportModule = function(name, that, t){
    if("object"==typeof exports&&"undefined"!=typeof module)
        module.exports=t();
    else if("function"==typeof define&&define.amd)
        define([],t);
    else{
        var r;
        r="undefined"!=typeof window?window:"undefined"!=typeof global?global:"undefined"!=typeof self?self:that,r[name]=t()
    }
};
// 加载模块函数
window.importModule = function(name){
    var m;
    if(("object"==typeof exports&&"undefined"!=typeof module) || ("function"==typeof define&&define.amd)){
        m = require(name);
    }else{
        if("undefined"!=typeof window){
            m = window[name];
        }else{
            m = global[name];
        }
    }
    return m;
};

!function(t){
    exportModule("server", this, t);
}(function () {
var server = {
    connect: _connect,          // wsUrl
    send: _send,                // obj: messageID, body, onMessage(messageID, body), onTimeout
    disconnect: _disconnect,    //
    on: _on,    //
    off: _off,    //
    emit: _emit,    //
    djb_hash: _djb_hash,
};
var _wsUrl = null,
    _wsObj = null,
    _wsAct = null,      // url, onOpen(e), onClose(e), onMessage(messageID, body)
    _cbNext = 0,
    _connectState = 0,  // 0 未连接 1已发起连接 2连接成功 3登录成功
    _eventMap = {},
    _cbMap = {};


var _Listener = {
    onConnected: function(){
        console.log("onConnected");
    },
    onDisConnect: function(){
        console.log("onDisConnect");
    },
    onOpen: function (e) {
        if(_wsAct){
            _wsAct.onOpen(e);
        }
    },
    onClose: function (e) {
        _wsObj = null;
        if(_wsAct){
            _wsAct.onClose(e);
        }
        _clear();
    },
    onMessage: function (e) {
        if( _isArrayBuffer(e.data) ){
            _onParse(e.data);
        }else{
            var fileReader = new FileReader();
            fileReader.onload = function() {
                _onParse(this.result);
            };
            fileReader.readAsArrayBuffer(e.data);
        }
    },
    onError: function (e) {
        this.onClose(e);
    },
};
function _connect(wsUrl){
    console.log("connect to " + wsUrl);
    _wsUrl = wsUrl;
    _connectState = 1;
    _open({
        url: wsUrl,
        onOpen: function (e) {
            _connectState = 2;
            console.log("connect OK " + wsUrl);
            // 先发送消息验证连接
            var obj = {
                messageID : 0x123,
                body : new ArrayBuffer(0),
                onMessage: function(messageID, body){
                    console.log("messageID 0x"+messageID.toString(16)+" body: "+body);
                    _emit("ServerConnectOpen", wsUrl);
                },
                onTimeout: function(){
                    console.log("0x"+this.messageID.toString(16)+" request failed ");
                    _disconnect();
                }
            };
            _send(obj);
            //_emit("ServerConnectOpen", wsUrl);
        },
        onClose: function (e) {
            _connectState = 0;
            console.log("connect Close " + wsUrl);
            _emit("ServerConnectClose", wsUrl);
        },
        onMessage: function (messageID, body) {
            _emit(messageID, body);
        }
    });
}
function _open(wsAct) {
    _clear();
    if(_wsObj !== null){
       _wsObj.close();
    }
    _on("ServerConnectOpen", _Listener.onConnected, _Listener);
    _on("ServerConnectClose", _Listener.onDisConnect, _Listener);
    _wsAct = wsAct;
    var ws = new WebSocket(wsAct.url);
    _wsObj = ws;
    ws.binaryType = "arraybuffer";  // arraybuffer or blob
    ws.onopen = function (e) { _Listener.onOpen(e); };
    ws.onclose = function (e) { _Listener.onClose(e); };
    ws.onmessage = function (e) { _Listener.onMessage(e); };
    ws.onerror = function (e) { _Listener.onError(e); };
}
function _send(obj){
    var messageID = obj.messageID;
    var body = obj.body;
    var callbackID = 0;
    if(typeof(obj.onMessage) !== "undefined"){
        callbackID = _next();
        _cbMap[callbackID] = { obj:obj, callbackID:callbackID, messageID:messageID, t:_time() };
    }
    if(typeof body === "string"){
        body = _str2ab(body);
    }
    var send_buf = new ArrayBuffer(8 + body.byteLength);
    var view = new DataView(send_buf);
    view.setUint32(0, callbackID);
    view.setUint32(4, messageID);
    for(var i=0; i<body.byteLength; ++i){
        view.setUint8(8+i, body[i]);
    }
    _wsObj.send(send_buf);
    console.log("send " + (typeof send_buf));
}
function _disconnect(){
    _wsUrl = null;
    _connectState = 0;
    _close();
}
function _close() {
    if(_wsObj !== null){
        _wsObj.close();
    }
}
function _clear() {
    _cbMap = {};
    _wsAct = null;
}
function _onParse(arrayBuffer){
    var view = new DataView(arrayBuffer);
    var callbackID = view.getUint32(0);
    var messageID = view.getUint32(4);
    var body = arrayBuffer.slice(8);
    return _onRecv(callbackID, messageID, body);
}
function _onRecv(callbackID, messageID, body){
    var cbInfo = _cbMap[callbackID];
    if(cbInfo){
        cbInfo.obj.onMessage(messageID, body);
        delete _cbMap[callbackID];
    }else {
        _wsAct.onMessage(messageID, body);
    }
}
function _next() {
   var n = _cbNext + 1;
   if(n > 65535){
       n = 1;
   }
   _cbNext = n;
   return n;
}
// callback: id, data
function _on(id, callback, target){
    var funcMap = _eventMap[id];
    if(typeof funcMap === "undefined"){
        funcMap = {};
        var targetArray = new Array();
        targetArray.push(target);
        funcMap[callback.name] = targetArray;
        _eventMap[id] = funcMap;
    }else{
        var targetArray = funcMap[callback.name];
        if(typeof targetArray === "undefined"){
            targetArray = new Array();
            targetArray.push(target);
            funcMap[callback.name] = targetArray;
        }else{
            for(var i=0,l=targetArray.length; i<l; ++i){
                if(target === targetArray[i]){
                    return;
                }
            }
            targetArray.push(target);
        }
    }
}
function _off(id, callback, target){
    var funcMap = _eventMap[id];
    if(typeof funcMap !== "undefined"){
        var targetArray = funcMap[callback.name];
        if(typeof targetArray !== "undefined"){
            for(var i=0,l=targetArray.length; i<l; ++i){
                if(target === targetArray[i]){
                    targetArray.splice(i, 1);
                    if(targetArray.length() === 0){
                        delete funcMap[callback.name];
                    }
                    return;
                }
            }
        }
    }
}
function _emit(id, data){
    var funcMap = _eventMap[id];
    if(typeof funcMap === "undefined"){
        console.log("can not find event for " + id + " " + JSON.stringify(data));
    }else{
        for(var name in funcMap){
            var targetArray = funcMap[name];
            for(var i=0,l=targetArray.length; i<l; ++i){
                var target = targetArray[i];
                var callback = target[name];
                callback.call(target, id, data);
            }
        }
    }
}
function _djb_hash(chars){
   if (typeof chars === 'string') {
       chars = chars.split('').map(function(str){
           return str.charCodeAt(0);
       });
   }
   if (!Array.isArray(chars)) {
       throw new Error('input must be a string or an array');
   }
   var h = chars.reduce(function(prev, curr){
       return ((prev << 5) + prev) + curr;
   }, 5381);
   return (h & 0x7FFFFFFF);
}
function _time(){ return Date.parse( new Date())/1000; }
function _timems() { return new Date().getTime(); }
function _ab2str(buf) { return String.fromCharCode.apply(String, new Uint8Array(buf)); }
function _str2ab(str) {
   var buf = new ArrayBuffer(str.length); // 1 bytes for each char
   var bufView = new Uint8Array(buf);
   for (var i=0, strLen=str.length; i<strLen; i++) {
       bufView[i] = str.charCodeAt(i);
   }
   return buf;
}
function _str2abOffset(str, buf, offset){
   var bufView = new Uint8Array(buf);
   for (var i=0, strLen=str.length; i<strLen; i++) {
       bufView[offset+i] = str.charCodeAt(i);
   }
}
function _isArrayBuffer(mix){ return Object.prototype.toString.call(mix) === "[object ArrayBuffer]"; }

return server;
});


