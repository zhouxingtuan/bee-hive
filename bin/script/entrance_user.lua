--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/6/8
-- Time: 下午11:50
-- To change this template use File | Settings | File Templates.
--

local config = {
	logLevel = "debug";	-- { "trace", "debug", "info", "warn", "error", "fatal" }
	moduleName = "user";
	logRoot = "log";
	maxFileSize = 100*1024*1024;
}
package.loaded["config"] = config

require("script.main")
-- 入口文件
require("script.user.init")
