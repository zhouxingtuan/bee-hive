local class = require("class")

local map = class("map")
local pairs = pairs
local util = require("util")

function map:ctor()
    self.m_count = 0
    self.m_data = {}
end

function map:dtor()
    self:clear()
end

function map:clear()
    for k,_ in pairs(self) do
        if nil ~= self.m_data[k] then
            self.m_data[k] = nil
        end
    end
    self.m_count = 0
end

function map:insert(k,v)
    if nil == self.m_data[k] then
        self.m_data[k] = v
        self.m_count = self.m_count + 1
    end
end

function map:remove(k)
    if nil ~= self.m_data[k] then
        self.m_data[k] = nil
        if self.m_count >0 then
            self.m_count = self.m_count - 1
        end
    end
end

function map:getvalue(k)
    if nil ~= self.m_data[k] then
        return self.m_data[k]
    end
end

function map:getAll()
    return self.m_data;
end

function map:exist(k)
    return (nil ~= self.m_data[k] and true) or false
end

function map:size()
    return self.m_count
end

function map:shuffle()
    local tb = {}
    for _,v in pairs(self.m_data) do
        table.insert(tb,v)
    end
    util.shuffle(tb)
    return tb
end

return map

