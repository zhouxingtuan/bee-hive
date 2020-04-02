
var wsUrl = "ws://192.168.50.184:8001"
server.connect(wsUrl)

var WBListener = {
    connectOpen: function(){
        console.log("connectOpen "+wsUrl);
        var obj = {
            messageID : 0x888,
            body : JSON.stringify({
                Test: "Hello World!",
            }),
            onMessage: function(messageID, body){
              console.log("messageID response 0x"+messageID.toString(16)+" body: "+server.ab2str(body));
            },
            onTimeout: function(){
              console.log("0x"+this.messageID.toString(16)+" request failed ");
            }
        };

        server.send(obj);
    },
};
server.on("ServerConnectOpen", WBListener.connectOpen, WBListener);

