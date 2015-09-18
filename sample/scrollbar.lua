

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
    local css = string.format("{width:%fpx; height:%fpx;}", w, slider_h)
    self.slider_:applyCSS(css)
    self.slider_area_:applyCSS(css)
    css = string.format("{left:%fpx; top:%fpx;}", 0, slider_t)
    self.slider_:applyCSS(css)
    self.slider_area_:applyCSS(css)
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
    local css = string.format("{width:%fpx; height:%fpx;}", slider_w, h)
    self.slider_:applyCSS(css)
    self.slider_area_:applyCSS(css)
    css = string.format("{left:%fpx; top:%fpx;}", slider_l, 0)
    self.slider_:applyCSS(css)
    self.slider_area_:applyCSS(css)
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

    self.background_ = self:getChildAt(0)
    self.slider_ = self:getChildAt(1)
    self.slider_area_ = self:getChildAt(2)
    self.slider_area_.parent_ = self

    self.min_ = 0
    self.max_ = 0
    self.view_ = 0
    self.cur_ = 0
end

function mod.onSizeChanged(self, w, h)
    local css = string.format("{width:%dpx; height:%dpx;}", w, h)
    self.background_:applyCSS(css)
    updateSlider(self)
end

--slider响应
--self是 area
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