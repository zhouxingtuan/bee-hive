
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
connection.on('binary', function(inStream) {
  // 创建空的buffer对象，收集二进制数据
  var data = new Buffer(0)
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
  })
})
connection.on('connection', function(conn) {
  console.log('connection open OK', conn)
  // 编写socket连接到access服务


})
connection.on('connect', function(code) {
  console.log('connect open', code)
  // 编写socket连接到access服务


})
connection.on('close', function(code) {
  console.log('connect close', code)
  // 这里需要关闭到access的socket连接

})
connection.on('error', function(code) {
  console.log('connect error', code)
  // 某些情况如果客户端多次触发连接关闭，会导致connection.close()出现异常，这里try/catch一下
  try {
    connection.close()
  } catch (error) {
    console.log('close exception', error)
  }
})
}).listen(listen_port, listen_host)