require("eventbus")

local GameHandler = eventbus.class("GameHandler")


function GameHandler:onEvent(event)
    print (event.name)
    bus:delHandler(handler2)
end

handler1 = GameHandler.new()
handler2 = GameHandler.new()
bus = eventbus.EventBus.new()

bus:addHandler(handler1)
bus:addHandler(handler2)

bus:postEvent({name=1, value=2})
