--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/6/6
-- Time: 上午5:41
-- To change this template use File | Settings | File Templates.
--

local config = {
	log_level = "debug";	-- { "trace", "debug", "info", "warn", "error", "fatal" }
	groups = {

	};
	moduleName = "test";
	dataRoot = "data";
	logRoot = "log";
}
package.loaded["config"] = config

require("script.main")
require("test")
