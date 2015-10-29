
local mod = {}

--设置回调
local function setDelegate(self, delegate)
    self.onDelegate_ = delegate
end
--设置item文本内容
local function setText(self, im, text)
    local item = self.items_[im]
    if item then
        item:setText(text)
    end
end

--选中指定项 不发送回调
local function switchItem(self, im)
    local item = self.items_[im]
    if item and not item:isSelected() then
        item:setSelected(true)
        for i = 1, 5 do
            if i ~= im then
                self.items_[i]:setSelected(false)
            end
        end
    end
end

--这里的self是指 被点击的item
local function itemSelected(self)
    if self:isSelected() then
        return
    end

    local ctrl = self:getParent()
    local im = 0
    for i = 1, 5 do
        if self == ctrl.items_[i] then
            ctrl.items_[i]:setSelected(true)
            im = i
        else
            ctrl.items_[i]:setSelected(false)
        end
    end
    if type(ctrl.onDelegate_) == "function" then
        ctrl:onDelegate_(im)
    end
end

function mod.onCreate(self) 
    self.setDelegate = setDelegate
    self.setText = setText
    self.switchItem = switchItem

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