require("class")

local map = require("map")

local set = class("set", map)

function set:ctor()
    set.super.ctor(self)
end

function set:insert(v)
    set.super.insert(self,v,v)
end

return set

