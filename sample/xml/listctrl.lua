
local mod = {}


local function addItemChild(self, item, ln, col, common, highlight, press, disable)
    local im = self.items_[item]
    if im then
        im:addItem(ln, col, common, highlight, press, disable)
    end
    --更新所有的item高度
    local l, t = 0, 0
    for i = 1, 5 do
        local item = self.items_[i]
        local w, h = item:getSize()
        item:setLoc(l, t)
        t = t + h
    end
    --更新垂直滚动值
    self:setScrollInfo(t, false)
end 

local function scrollItem(self, im)
    local item = self.items_[im]
    if item then
        local l, t = item:getLoc()
        self:setScrollPos(t, false)
    end
end

local function setText(self, im, text)
    local item = self.items_[im]
    if item then
        item:setText(text)
    end
end

local function setDelegate(self, delegate)
    self.onDelegate_ = delegate
end

local function getItem(self, cur)
    for i = 1, 5 do
        local item = self.items_[i]
        local w, h = item:getSize()
        local l, t = item:getLoc()
        local b = t + h
        if cur >= t and cur < b then
            return i
        end
    end

    return 0
end

function mod.onCreate(self)
    --public method
    self.setText = setText
    self.addItem = addItemChild
    self.scrollItem = scrollItem
    self.setDelegate = setDelegate
    self.getItem = getItem

    --当前所有的listitem
    self.items_ = {}
    local item_w, item_h = 736 + 18, 52
    local item_l, item_t = 0, 0
    for i = 1, 5 do
        local item = self:getChildAt(i - 1)
        item:setLoc(item_l, item_t)
        if i == 5 then --最后一项的高度固定是378      
            item_h = 378
        end
        item:setSize(item_w, item_h)

        item_t = item_t + item_h
        self.items_[i] = item
    end
end

function mod.onScrollRange(self, min, max, view, horiz)
    if not horiz and type(self.onDelegate_) == "function" then
        self:onDelegate_("range", min, max, view)
    end
end

function mod.onScrollPos(self, cur, horiz)
    if not horiz and type(self.onDelegate_) == "function" then
        self:onDelegate_("pos", cur)
    end
end


return mod;