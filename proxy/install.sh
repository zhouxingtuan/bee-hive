
cd /data/

wget https://npm.taobao.org/mirrors/node/v12.10.0/node-v12.10.0-linux-x64.tar.xz

tar -xvf node-v12.10.0-linux-x64.tar.xz

ln -s /data/node-v12.10.0-linux-x64/bin/node /usr/bin/node

ln -s /data/node-v12.10.0-linux-x64/bin/npm /usr/bin/npm

ln -s /data/node-v12.10.0-linux-x64/bin/npx /usr/bin/npx

npm install nodejs-websocket


