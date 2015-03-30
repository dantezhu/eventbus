--=============================================================================
--
--     FileName: eventbus.lua
--         Desc: 
--
--       Author: dantezhu
--        Email: zny2008@gmail.com
--     HomePage: http://www.vimer.cn
--
--      Created: 2014-11-09 18:50:32
--      Version: 0.0.1
--      History:
--               0.0.1 | dantezhu | 2014-11-09 18:50:32 | init
--
--=============================================================================

local function class(classname, super)
    local superType = type(super)
    local cls

    if superType ~= "function" and superType ~= "table" then
        superType = nil
        super = nil
    end

    if superType == "function" or (super and super.__ctype == 1) then
        -- inherited from native C++ Object
        cls = {}

        if superType == "table" then
            -- copy fields from super
            for k,v in pairs(super) do cls[k] = v end
            cls.__create = super.__create
            cls.super    = super
        else
            cls.__create = super
            cls.ctor = function() end
        end

        cls.__cname = classname
        cls.__ctype = 1

        function cls.new(...)
            local instance = cls.__create(...)
            -- copy fields from class to native object
            for k,v in pairs(cls) do instance[k] = v end
            instance.class = cls
            instance:ctor(...)
            return instance
        end

    else
        -- inherited from Lua Object
        if super then
            cls = {}
            setmetatable(cls, {__index = super})
            cls.super = super
        else
            cls = {ctor = function() end}
        end

        cls.__cname = classname
        cls.__ctype = 2 -- lua
        cls.__index = cls

        function cls.new(...)
            local instance = setmetatable({}, cls)
            instance.class = cls
            instance:ctor(...)
            return instance
        end
    end

    return cls
end


local M = class("EventBus")

function M:ctor()
   self.handlers = {}
   self.events = {}
   self.schedEntry = nil
end

function M:start()
    if not self.schedEntry then
        self.schedEntry = cc.Director:getInstance():getScheduler():scheduleScriptFunc(
            function ()
                self:loopEvents()
            end,
        0,false)
    end
end

function M:stop()
    if self.schedEntry then
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.schedEntry)
        self.schedEntry = nil
    end
end

function M:isRunning()
    return self.schedEntry ~= nil
end

function M:loopEvents()
    local events = self.events
    self.events = {}

    for idx,event in ipairs(events) do
        self:onEvent(event)
    end
end

function M:clearEvents()
    self.events = {}
end

function M:addHandler(handler)
    table.insert(self.handlers, handler)
end

function M:delHandler(handler)
    local i = 1
    while i <= #self.handlers do
        local v = self.handlers[i]
        if v == handler then
            table.remove(self.handlers, i)
        else
            i = i + 1
        end
    end
end

function M:onEvent(event)
    local tmpHandlers = {}
    for i, val in ipairs(self.handlers) do
        table.insert(tmpHandlers, val)
    end

    for i, handler in ipairs(tmpHandlers) do
        local found = false
        for tmpi, tmpval in ipairs(self.handlers) do
            if tmpval == handler then
                -- 必须要找到才行
                found = true
            end
        end
        if found == true then
            handler:onEvent(event)
        end
    end
end

function M:postEvent(event)
    table.insert(self.events, event)
end

return M
