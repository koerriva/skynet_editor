table = {"1"}

print("Hello Lua!哈喽撸啊")

function update()
    print("Update ...")
    r = GetMousePos()
    print("Pos ",r.x)
    t = GetDeltaTime()
    print("Frame Time ",t)
end