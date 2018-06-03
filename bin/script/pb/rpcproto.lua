--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/6/3
-- Time: 下午2:51
-- To change this template use File | Settings | File Templates.
--

local rpcproto = {
	-- 系统使用命令
	{0x98, "pb", "rpc.rpc_push", "rpc.rpc_empty"};
	{0x99, "json", "", ""};
	-- 注册账号
	{0x101,		"pb", 	"rpc", "rpc"};
	-- 登录服务

	-- 账号服务

	-- user 服务
	{0x1001, "json", "", ""};
	{0x6666, "json", "", ""};
	{0x6667, "json", "", ""};
	{0x6668, "json", "", ""};
	-- game 服务
	{0x5001,		"pb", 	"rpc", "rpc"};
}

return rpcproto
