
local mod = {}

--self代表root标签
function mod.onCreate(self)
    --self:setOpacity(0.5)
    local input = self:getChildAt(0)
    input:setContent("aaaaabbb")
end

--点击tab页产生的事件
local function onTabDelegate(self, im)
    local scroll_list = self:getParent()
    scroll_list.list_:scrollItem(im)
end

--list 滚动变化产生的事件
local function onListDelegate(self, action, min, max, view)
    local scroll_list = self:getParent()
    if "range" == action then
        scroll_list.bar_:setScrollRange(min, max, view)
    elseif "pos" == action then
        local cur = min
        scroll_list.bar_:setScrollPos(cur)
    end 
end

--scrollbar 鼠标产生的高级事件
local function onBarDelegate(self, action, cur)
    local scroll_list = self:getParent()
    --拖动滚动滑块
    if "drag" == action then
        scroll_list.list_:setScrollPos(cur, false)
    --鼠标在滑块上弹起 要将tab切换到list首个显示的item
    elseif "release" == action then
        scroll_list.tab_:switchItem(scroll_list.list_:getItem(cur))
    end
end

function mod.onListCreate(self)
    self:setRect(0, 0)
    self:setColor(0xffebeffa)
    
    self.tab_ = self:getChildAt(0)
    self.list_ = self:getChildAt(1)
    self.bar_ = self:getChildAt(2)
    --垂直滚动条
    self.bar_:setStyle(false)
    --先注册回调
    self.list_:setDelegate(onListDelegate)
    self.bar_:setDelegate(onBarDelegate)   
    self.tab_:setDelegate(onTabDelegate)
    --设置tab页显示
    self.tab_:setText(1,"常用")
    self.tab_:setText(2,"上网")
    self.tab_:setText(3,"系统")
    self.tab_:setText(4,"软件")
    self.tab_:setText(5,"其他")
    --设置list显示
    self.list_:setText(1,"常用")
    self.list_:setText(2,"上网")
    self.list_:setText(3,"系统")
    self.list_:setText(4,"软件")
    self.list_:setText(5,"其他")
    --设置list上的按钮
    self.list_:addItem(1, 1, 1, "common","highlight", "press", "disable")
    self.list_:addItem(1, 1, 2, "common","highlight", "press", "disable")
    self.list_:addItem(1, 1, 3, "common","highlight", "press", "disable")
    self.list_:addItem(1, 1, 4, "common","highlight", "press", "disable")
    self.list_:addItem(1, 1, 5, "common","highlight", "press", "disable")
    self.list_:addItem(1, 1, 6, "common","highlight", "press", "disable")
    self.list_:addItem(1, 1, 7, "common","highlight", "press", "disable")
    self.list_:addItem(1, 1, 8, "common","highlight", "press", "disable")

    self.list_:addItem(2, 1, 1, "common","highlight", "press", "disable")
    self.list_:addItem(2, 2, 1, "common","highlight", "press", "disable")
    self.list_:addItem(3, 1, 1, "common","highlight", "press", "disable")
    self.list_:addItem(4, 1, 1, "common","highlight", "press", "disable")
    self.list_:addItem(5, 1, 1, "common","highlight", "press", "disable")
    self.list_:addItem(5, 2, 1, "common","highlight", "press", "disable")
    self.list_:addItem(5, 2, 2, "common","highlight", "press", "disable")
end

function mod.onListSizeChanged(self, w, h)
    --布局滚动条 TAB list
    local tab_width = 91
    self.tab_:setSize(tab_width, h)
    self.list_:setLoc(112, 0)
    self.list_:setSize(736 + 18, h)
    local bar_const = 12
    self.bar_:setLoc(w - bar_const, bar_const)
    self.bar_:setSize(bar_const, h - bar_const - bar_const)
end




return mod;