uuid = {}

local function id_seq()
    local i = 0
    while true do
        i = i+1
        coroutine.yield(i)
    end
end

local id_generator = coroutine.create(id_seq)

---@type number
function uuid.next()
    local r,id = coroutine.resume(id_generator)
    return id
end

return uuid