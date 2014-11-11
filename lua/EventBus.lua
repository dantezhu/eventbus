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
end

function M:addHandler(handler)
    table.insert(self.handlers, handler)
end

function M:delHandler(handler)
    for name,val in pairs(self.handlers) do
        if val == handler then
            self.handlers[name] = nil
        end
    end
end

function M:postEvent(...)
    -- 直接就执行即可，因为lua一定在主线程

    local tmpHandlers = {}
    for key,val in pairs(self.handlers) do
        table.insert(tmpHandlers, val)
    end

    for key,handler in pairs(tmpHandlers) do
        local found = false
        for tmpkey,tmpval in pairs(self.handlers) do
            if tmpval == handler then
                -- 必须要找到才行
                found = true
            end
        end
        if found == true then
            handler:onEvent(...)
        end
    end
end

return M
