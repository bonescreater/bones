
local mod = {}
--136 - 84
--设置指定的行列元素
local function addItem(self, ln, col, common, highlight, press, disable)
    --按钮的高度就是一行的高度
    local btn_wh = 84;
    local item = bones.createObject(self, "button")
    item:setBitmap(common, highlight, press, disable)
    item:setSize(btn_wh, btn_wh)
    item:setLoc(col * btn_wh, ln * btn_wh)
    
    
    self.items_[ln][col] = item

end

local function setTitle(self, text)
    self.title:setContent(text)
end

function mod.onCreate(self)
    self.addItem = addItem
    self.setTitle = setTitle
    self.title_ = self:getChildAt(0)
    self.btns_ = {}
end

function mod.onSizeChanged(self, w, h)
    self.title_:setSize(w, h)
end

return mod;