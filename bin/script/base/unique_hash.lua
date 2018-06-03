--
-- Created by IntelliJ IDEA.
-- User: zxt
-- Date: 2017/9/5
-- Time: 14:41
-- To change this template use File | Settings | File Templates.
--

local table = table

local unique_hash = class("unique_hash")

function unique_hash:ctor()
    self.m_hash = {}
    self.m_array = {}
    self.m_index = 1
end

function unique_hash:add(key, value)
    local m_hash = self.m_hash
    local m_array = self.m_array
    local info = m_hash[key]
    if info == nil then
        table.insert(m_array, value)
    else
        for k,v in ipairs(m_array) do
            if info == v then
                m_array[k] = value
                break
            end
        end
    end
    m_hash[key] = value
end
function unique_hash:remove(key)
    local m_hash = self.m_hash
    local m_array = self.m_array
    local info = m_hash[key]
    if info ~= nil then
        m_hash[key] = nil
        for k,v in ipairs(m_array) do
            if v == info then
                table.remove(m_array, k)
                break
            end
        end
    end
end
function unique_hash:next()
    local m_array = self.m_array
    if #m_array == 0 then
        return
    end
    local m_index = self.m_index
    if m_index > #m_array then
        m_index = 1
    end
    local info = m_array[m_index]
    self.m_index = m_index + 1
    return info
end
function unique_hash:get(key)
    return self.m_hash[key]
end
function unique_hash:getAll()
    return self.m_array
end

return unique_hash
