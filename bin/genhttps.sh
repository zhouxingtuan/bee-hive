
#// 生成顶级CA的公钥证书和私钥文件，有效期15年（RSA 2048bits，指定）
openssl req -newkey rsa:2048 -x509 -days 5480 -keyout CARoot2048.key -out CARoot2048.crt

#// 为顶级CA的私钥文件去除保护口令
openssl rsa -in CARoot2048.key -out CARoot2048.key

