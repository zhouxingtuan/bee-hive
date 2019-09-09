
var access_host = "192.168.50.184";
var access_port = 30001;
var listen_host = "192.168.50.184";
var listen_port = 8001;

//var server = import("server")
//console.log(typeof server)
//console.log(typeof server.listen)
//server.listen(listen_port, listen_host);

const ws = require('nodejs-websocket')
let server = ws.createServer(connection => {
// 如果到access的连接还没建立，那么从ws收到的数据需要先缓存下来
var tempCache = new Buffer(0);
var acceptConn;
var acceptState = 0;
function _closeConnection(){
    if(connection != null){
        var c = connection;
        connection = null;
        c.close();
    }
}
function _closeAcceptConn(){
    if(acceptConn != null){
        var ac = acceptConn;
        acceptConn = null;
        ac.close();
    }
}
function _openAcceptConn(){
    if(acceptConn != null){
        console.log("connect is already open");
        return;
    }
    acceptConn = net.connect({port: access_port, host: access_host, }, function() {
        acceptState = 2;
        console.log('connect OK to Access');
        // tempCache的数据发送到access

    });
    acceptConn.on('data', function(data) {
        console.log("recv data", data.byteLength);
        // 发送消息到 connection 客户端

    });
    acceptConn.on('end', function() {
        acceptState = 3;
        console.log('close connect to access');
        acceptConn = null;
        _closeConnection();
    });
    acceptState = 1;
}
function _sendDataToAccess(data){
    if(acceptState == 0){
        // 第一个数据包丢弃 8 字节，剩余的数据进入tempCache

    }else if(acceptState == 1){
        // 数据进入tempCache

    }else if(acceptState == 2){
        // 直接转发数据

    }else{
        // 3 不做处理
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
        console.log("receive data " + data.byteLength);
        _sendDataToAccess(data);
        if(acceptConn == null){
            _openAcceptConn();
        }
    })
})
connection.on('close', function(code) {
    console.log('connect close', code)
    // 这里需要关闭到access的socket连接
    connection = null;
    _closeAcceptConn();
})
connection.on('error', function(code) {
    console.log('connect error', code)
    // 某些情况如果客户端多次触发连接关闭，会导致connection.close()出现异常，这里try/catch一下
    try {
        _closeConnection();
    } catch (error) {
        console.log('close exception', error)
    }
})
}).listen(listen_port, listen_host)