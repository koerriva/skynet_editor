local tiny = require("tiny")
local uuid = require("id")

local inputSystem = tiny.processingSystem()
inputSystem.filter = tiny.requireAll("isActive")
function inputSystem:process(e,dt)
    if(e.isActive)then

    end
end

neural = {}

math.randomseed(1234)

local world = tiny.world(inputSystem)

local function colorRGBA(r,g,b,a)
    local c = color.new()
    c.r = r
    c.g = g
    c.b = b
    c.a = a
    return c
end
for i = 1, 10000 do
    local id = uuid.next()
    local isActive = math.random(0,1)==1
    local pos = vec2.new()
    pos.y = math.random(-2000,2000)
    pos.x = math.random(-2000,2000)
    local _color = colorRGBA(math.random(1,100),12,3,math.random(122,250))
    local radius = 32
    neural[id] = { id=id,isActive=isActive,pos=pos,color= _color,radius=radius }
    world.addEntity(world,neural[id])
end

local time = 0
function update()
    --local deltaTime = GetDeltaTime()
    --time = deltaTime + time
    --if time >= 1.0 then
    --    time=0
    --    local gc_size = collectgarbage("count")
    --    print("GC Size(KB)",gc_size)
    --    local pos = GetMousePos()
    --    print(pos.x,pos.y)
    --end
    --world:update(deltaTime)
end

print("哈喽 Lua!")
