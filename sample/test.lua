local mod = {}


function mod.onCreate(self)
    local btn1 = self:getChildAt(0)
    local btn2 = self:getChildAt(1)

    btn1:setBitmap("common", "highlight", "press", "disable")
    btn2:setBitmap("common", "highlight", "press", "disable")
end


return mod