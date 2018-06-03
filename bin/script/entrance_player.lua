--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/6/8
-- Time: 下午11:40
-- To change this template use File | Settings | File Templates.
--

local path = require("script.path")

local config = {
	log_level = "debug";	-- { "trace", "debug", "info", "warn", "error", "fatal" }
	path = path;
	groups = {

	};
	moduleName = "player";
	dataRoot = "data";
	logRoot = "log";
}
package.loaded["config"] = config

require("script.main")
require("player")
