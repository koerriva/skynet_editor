table = {id=1,name="yyy"}

time = 0

function id_seq()
    local i = 0
    while true do
        i = i+1
        coroutine.yield(i)
    end
end

id_generator = coroutine.create(id_seq)

function update()
    time = GetDeltaTime() + time
    if time >= 1.0 then
        time=0
        local gc_size = collectgarbage("count")
        print("GC Size(KB)",gc_size*1024)
    end
    --local r,id = coroutine.resume(id_generator)
    --print(r,id)

    --print("update ",3)
end

print("哈喽 Lua!")
