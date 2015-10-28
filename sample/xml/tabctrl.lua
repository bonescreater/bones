
local mod = {}

--这里的self是指 被点击的item
local function itemSelected(self)
    local ctrl = self:getParent()
    for i = 1, 5 do
        if self == ctrl.items_[i] then
            ctrl.items_[i]:setSelected(true)
        else
            ctrl.items_[i]:setSelected(false)
        end
    end
end

function mod.onCreate(self)
    --self.appendItem = appendItem
    self.items_ = {}

    local item_w, item_h = 91, 62
    local item_l, item_t = 0, 0
    for i = 1, 5 do
        local item = self:getChildAt(i)
        item:setLoc(item_l, item_t)
        item:setSize(item_w, item_h)
        item:setDelegate(itemSelected)
        item_t = item_t + item_h
        self.items_[i] = item

    end

    self.items_[1]:setText("常用")
    self.items_[2]:setText("上网")
    self.items_[3]:setText("系统")
    self.items_[4]:setText("软件")
    self.items_[5]:setText("其他")
    
    self.line_ = self:getChildAt(0)
    self.line_:setColor(0xffd5dadb)

    self:setRect(0, 0)
    self:setColor(0xffd6e6f3)

    itemSelected(self.items_[1])
end

function mod.onSizeChanged(self, w, h)
    self.line_:setSize(w, h)
    self.line_:setLine(w - 1, 0, w - 1, h)
end

return mod;