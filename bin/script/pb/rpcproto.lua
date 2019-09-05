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

	{0x3001, "json", "", ""};

}

return rpcproto
