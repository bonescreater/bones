
local mod = {}
local function PrintItem(ob)
    print(ob)
end

local function appendItem(self)
    local im = bones.createObject(self, "listitem", PrintItem)
    if im then
        local total = 0
        local n = #self.list_items_
        for i = 1, n, 1 do
            local item = self.list_items_[i]
            local w, h = item:getSize()
            total = total + h
        end
        im:setLoc(0, total);
        --im的大小自己计算
        self.list_items_[n + 1] = im
        local w, h = im:getSize()
        total = total + h
        --更新垂直滚动值
        self:setScrollInfo(total, false)
    end
end

local function setItemTitle(self, item, text)
    local im = self.list_items_[item]
    if im then
        im:setTitle(text)
    end
end

local function addItemChild(self, item, ln, col, common, highlight, press, disable)
    local im = self.list_items_[item]
    if im then
        im:addItem(ln, col, common, highlight, press, disable)
    end
end 

function mod.onCreate(self)
    --public method
    --增加一项
    self.appendItem = appendItem
    --设置子项的文本
    self.setItemTitle = setItemTitle
    --设置子项
    --当前所有的listitem
    self.list_items_ = {}
end



return mod;