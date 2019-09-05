--
-- Created by IntelliJ IDEA.
-- User: zxt
-- Date: 2017/10/10
-- Time: 18:49
-- To change this template use File | Settings | File Templates.
--

log_debug("hotupdate start")

package.loaded["redis"] = nil
package.loaded["mysql"] = nil
package.loaded["util"] = nil
package.loaded["server_type"] = nil

require("redis")
require("mysql")
require("util")
require("server_type")
