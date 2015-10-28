
local mod = {}

--self代表root标签
function mod.onCreate(self)


end

function mod.onListCreate(self)
    --
    print("onCreate")
    self:setRect(0, 0)
    self:setColor(0xffebeffa)
    self.tab_ = self:getChildAt(0)
    self.list_ = self:getChildAt(1)
    self.list_:appendItem()
end

function mod.onListSizeChanged(self, w, h)
    local tab_width = 91
    self.tab_:setSize(tab_width, h)
end

return mod;