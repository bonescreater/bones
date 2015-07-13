local mod = {}

function mod.onPrepare(self)
print("test prepare")
btn1 = bones.getLuaObjectByID("btn1")
close = bones.getLuaObjectByID("close")
btn1:applyTextClass("btn1")
close:applyTextClass("close")
end

return mod;