
var access_host = "192.168.50.184";
var access_port = 30005;
var listen_host = "192.168.50.184";
var listen_port = 8001;

//var server = import("server")
//console.log(typeof server)
//console.log(typeof server.listen)
//server.listen(listen_port, listen_host);

const COMMAND_REGISTER = 253;
const COMMAND_CLIENT_REQUEST = 1;

var net = require('net');
const ws = require('nodejs-websocket')
let server = ws.createServer(connection => {
// 如果到access的连接还没建立，那么从ws收到的数据需要先缓存下来
var tempCache = new Buffer(0);
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
   return chars.reduce(function(prev, curr){
       return ((prev << 5) + prev) + curr;
   }, 5381);
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
        ac.close();
    }
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
        // 验证成功的第一条消息，需要解析；然后通知客户端链接成功
        if(_acceptState == 2){

            _acceptState = 3;    // 前往access的连接已经验证成功
        }else{
            // 发送消息到 connection 客户端

        }
    });
    _acceptConn.on('end', function() {
        _acceptState = 8;    // 前往access的连接结束
        console.log('access connection close end');
        _acceptConn = null;
        _closeConnection();
    });
    _acceptState = 1;    // 前往access的连接已经创建
}
function _sendDataToAccess(data){
    var buf = _toArrayBuffer(data);
    var view = new DataView(buf);
    var callbackID = view.getUint32(0);
    var messageID = view.getUint32(4);
    if(_acceptState == 0){
        // 第一个数据包丢弃 8 字节，只获取并记录callbackID
        console.log("receive messageID 0x"+messageID.toString(16)+" callbackID "+callbackID.toString());
        _identifyCallbackID = callbackID;
        _identifyMessageID = messageID;
    }else if(_acceptState == 3){
        // 转发数据给access服务
        var length = 28 - 8 + data.byteLength;
        var send_buf = new ArrayBuffer(length);
        var view = new DataView(send_buf);
        view.setInt32(0, length);
        view.setUint32(4, COMMAND_CLIENT_REQUEST);
        view.setUint32(4, callbackID);
        view.setUint32(20, messageID);
        // 28往后的字节拼接data 8以后的数据

        send_buf = _toBuffer(send_buf);
        _acceptConn.write(send_buf);
    }else{
        // 不做处理
        console.log("current message to access not handle ", data.byteLength);
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