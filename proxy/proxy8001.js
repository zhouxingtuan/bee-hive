
var access_host = "192.168.50.184";
var access_port = 30005;
var listen_host = "192.168.50.184";
var listen_port = 8001;

const COMMAND_REGISTER = 253;
const COMMAND_CLIENT_REQUEST = 1;

var net = require('net');
const ws = require('nodejs-websocket')
let server = ws.createServer(connection => {
// 如果到access的连接还没建立，那么从ws收到的数据需要先缓存下来
var _tempCache = new ArrayBuffer(0);
var _acceptConn;
var _acceptState = 0;    // 连接初始状态
var _identifyCallbackID = 0;
var _identifyMessageID = 0;
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
function _toBuffer(ab) {
    var buf = new Buffer(ab.byteLength);
    var view = new Uint8Array(ab);
    for (var i = 0; i < buf.length; ++i) {
        buf[i] = view[i];
    }
    return buf;
}
function _toArrayBuffer(buf) {
    var ab = new ArrayBuffer(buf.length);
    var view = new Uint8Array(ab);
    for (var i = 0; i < buf.length; ++i) {
        view[i] = buf[i];
    }
    return ab;
}
function _sendIdentifyMessage(){
    var id = 1;
    var key = "22f100df9695ecaefd26e6a357eebf7f";
    var time = Date.parse( new Date())/1000;
    var temp = "0001-"+time.toString()+"-"+key;
    var magic = _djb_hash(temp);
    console.log("_sendIdentifyMessage", temp, "magic", magic);
    var length = 28 + 12;
    var send_buf = new ArrayBuffer(length);
    var view = new DataView(send_buf);
    view.setInt32(0, length);
    view.setUint32(4, COMMAND_REGISTER);
    view.setUint32(28, id);
    view.setUint32(32, time);
    view.setUint32(36, magic);
    send_buf = _toBuffer(send_buf);
    _acceptConn.write(send_buf);
}
function _closeConnection(){
    if(connection != null){
        console.log("websocket connection close");
        var c = connection;
        connection = null;
        c.close();
    }
}
function _closeAcceptConn(){
    if(_acceptConn != null){
        console.log("access connection close");
        var ac = _acceptConn;
        _acceptConn = null;
        ac.end();
    }
}
function _mergeArrayBufferTo(toBufView, index, fromBuf, fromOffset){
    var fromBuf_view = new DataView(fromBuf);
    for(var i=fromOffset; i<fromBuf.byteLength; ++i){
        toBufView.setUint8(index, fromBuf_view.getUint8(i));
        ++index;
    }
    return index;
}
function _appendTempCache(data){
    var buf = new ArrayBuffer(_tempCache.byteLength + data.byteLength);
    var view = new DataView(buf);
    var index = 0;
    index = _mergeArrayBufferTo(view, index, _tempCache, 0);
    var data_buf = _toArrayBuffer(data);
    index = _mergeArrayBufferTo(view, index, data_buf, 0);
    _tempCache = buf;
}
function _checkFullPacket(callback){
    do{
        if(_tempCache.byteLength < 28){
            return;
        }
        var view = new DataView(_tempCache);
        var length = view.getInt32(0);
        if(length < _tempCache.byteLength){
            return;
        }
        var send_buf = _tempCache.slice(0, length);
        _tempCache = _tempCache.slice(length);
        callback(send_buf);
    }while(true);
}
function _sendToClient(callbackID, messageID, send_buf){
    var buf = new ArrayBuffer(8 + send_buf.byteLength - 28);
    var view = new DataView(buf);
    view.setUint32(0, callbackID);
    view.setUint32(4, messageID);
    var index = 8;
    _mergeArrayBufferTo(view, index, send_buf, 28);
    buf = _toBuffer(buf);
    connection.sendBinary(buf);
}
function _sendDataToClient(data){
    _appendTempCache(data);
    _checkFullPacket(function(send_buf){
        // 验证成功的第一条消息，需要解析；然后通知客户端链接成功
        var view = new DataView(send_buf);
        if(_acceptState == 2){
            var destination = view.getUint32(12);
            if(destination > 0){
                console.log("access connection identify OK");
                _acceptState = 3;    // 前往access的连接已经验证成功
                // 发送消息通知客户端，连接建立完成
                _sendToClient(_identifyCallbackID, _identifyMessageID, send_buf);
            }else{
                console.log("access connection identify failed");
                _closeConnection();
            }
        }else{
            // 发送消息到 connection 客户端
            var callbackID = view.getUint32(8);
            var messageID = view.getUint32(20);
            _sendToClient(callbackID, messageID, send_buf);
        }
    });
}
function _openAcceptConn(){
    console.log("_openAcceptConn");
    if(_acceptConn != null){
        console.log("access connection is already open");
        return;
    }
    _acceptConn = net.connect({port: access_port, host: access_host, }, function() {
        _acceptState = 2;    // 前往access的连接已经成功
        console.log('access connection OK');
        // 发送验证信息，验证连接
        _sendIdentifyMessage();
    });
    _acceptConn.on('data', function(data) {
        console.log("access connection recv data", data.byteLength);
        _sendDataToClient(data);
    });
    _acceptConn.on('end', function() {
        _acceptState = 4;    // 前往access的连接结束
        console.log('access connection close end');
        _acceptConn = null;
        _closeConnection();
    });
    _acceptState = 1;    // 前往access的连接已经创建
}
function _sendDataToAccess(data){
    var buf = _toArrayBuffer(data);
    var buf_view = new DataView(buf);
    var callbackID = buf_view.getUint32(0);
    var messageID = buf_view.getUint32(4);
    if(_acceptState == 0){
        // 第一个数据包丢弃 8 字节，只获取并记录callbackID
        console.log("receive messageID 0x"+messageID.toString(16)+" callbackID "+callbackID.toString());
        if(messageID != 0x123){
            _closeConnection();
            return;
        }
        _identifyCallbackID = callbackID;
        _identifyMessageID = messageID;
    }else if(_acceptState == 3){
        // 转发数据给access服务
        var length = 28 + (buf.byteLength - 8);
        var send_buf = new ArrayBuffer(length);
        var view = new DataView(send_buf);
        view.setInt32(0, length);
        view.setUint32(4, COMMAND_CLIENT_REQUEST);
        view.setUint32(8, callbackID);
        view.setUint32(20, messageID);
        // 28往后的字节拼接data 8以后的数据
        var index = 28;
        for(var i=8; i<buf.byteLength; ++i){
            view.setUint8(index, buf_view.getUint8(i));
            ++index;
        }
        send_buf = _toBuffer(send_buf);
        _acceptConn.write(send_buf);
    }else{
        // 不做处理
        console.log("current message to access not handle ", buf.byteLength);
    }
}
connection.on('binary', function(inStream) {
    // 创建空的buffer对象，收集二进制数据
    var data = new Buffer(0);
    // 读取二进制数据的内容并且添加到buffer中
    inStream.on('readable', function() {
        var newData = inStream.read()
        if (newData){
            data = Buffer.concat([data, newData], data.length + newData.length)
        }
    })
    inStream.on('end', function() {
        // 读取完成二进制数据后，处理二进制数据
        //        process_my_data(data)
        console.log("websocket receive data " + data.byteLength);
        _sendDataToAccess(data);
        if(_acceptConn == null){
            _openAcceptConn();
        }
    })
})
connection.on('close', function(code) {
    console.log('websocket connect close', code)
    // 这里需要关闭到access的socket连接
    connection = null;
    _closeAcceptConn();
})
connection.on('error', function(code) {
    console.log('websocket connect error', code)
    // 某些情况如果客户端多次触发连接关闭，会导致connection.close()出现异常，这里try/catch一下
    try {
        _closeConnection();
    } catch (error) {
        console.log('websocket close exception', error)
    }
})
}).listen(listen_port, listen_host)
