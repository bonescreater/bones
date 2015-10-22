

local mod = {}
--滚动条
local function setSlave(self, slave)
    self.slave_ = slave
end

--设置水平还是垂直
local function setStyle(self, horiz)
    self.horiz_ = horiz
end

local function updateSliderV(self)
    -- view + min + max = bar.height
    local w, h = self:getSize()
    local view = self.view_
    local min = self.min_
    local max = self.max_
    local cur = self.cur_
    local total = view + min + max
    local scale_slider = view / total
    local slider_h = view / total * h
    local slider_t = (cur - min) / (max - min) * (h - slider_h)
    self.slider_:setSize(w, slider_h)
    self.slider_:setLoc(0, slider_t)
end

local function updateSliderH(self)
    -- view + min + max = bar.width
    local w, h = self:getSize()
    local view = self.view_
    local min = self.min_
    local max = self.max_
    local cur = self.cur_
    local total = view + min + max
    local scale_slider = view / total
    local slider_w = view / total * w
    local slider_l = (cur - min) / (max - min) * (w - slider_w)
    self.slider_:setSize(slider_w, h)
    self.slider_:setLoc(slider_l, 0)
end

local function updateSlider(self)
    if self.horiz_ then
        updateSliderH(self)
    else
        updateSliderV(self)
    end
end

local function setScrollPos(self, cur)
    self.cur_ = cur
    updateSlider(self)
end

local function setScrollRange(self, min, max, view)
     self.min_ = min
     self.max_ = max
     self.view_ = view
     local cur = self.cur_
     if cur > self.max_ then
        self.cur_ = self.max_
     end

     if cur < self.min_ then
        self.cur_ = self.min_
     end

    updateSlider(self)
end



function mod.onCreate(self)
    self.setScrollPos = setScrollPos
    self.setSlave = setSlave
    self.setScrollRange = setScrollRange
    self.setStyle = setStyle
    --底色
    self:setRect(0, 0)
    self:setColor(0xff000000)
    --滑块
    self.slider_ = self:getChildAt(0)
    self.slider_:setRect(0, 0)
    self.slider_:setColor(0xff00ff00)
    
    --滑块保留对父的引用 方便调用接口
    self.slider_.parent_ = self

    self.min_ = 0
    self.max_ = 0
    self.view_ = 0
    self.cur_ = 0
end

--滚动条大小改变了也要修改滑块的大小
function mod.onSizeChanged(self, w, h)
    updateSlider(self)
end


function mod.onSliderMouseDown(self, e)
    if e:isLeftMouse() then
        self.click_ = true
        self.last_x_, self.last_y_ = e:getRootLoc()
    end
end

function mod.onSliderMouseUp(self, e)
    if e:isLeftMouse() then
        self.click_ = false
    end
end

--self是滑块
function mod.onSliderMouseMove(self, e)   
    if self.click_ then
        local x, y = e:getRootLoc()
        local ydelta = y - self.last_y_
        local xdelta = x - self.last_x_
        local parent = self.parent_        
        local w, h = self:getSize()
        local cur = 0
        local horiz = parent.horiz_
        if horiz then
            cur = xdelta / w * (parent.max_ - parent.min_) + parent.cur_
        else
            cur = ydelta / h * (parent.max_ - parent.min_) + parent.cur_ 
        end
        parent.slave_:setScrollPos(cur, horiz)
    end

    self.last_x_, self.last_y_ = e:getRootLoc()
end

function mod.onSliderMouseLeave(self, e)
    self.click_ = false
    self.last_x_ = -1
    self.last_y_ = -1
end

return mod