
--垂直滚动条
local mod = {}

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
    css = string.format("{left:%fpx; top:%fpx;}", 0, slider_t)
    self.slider_:applyCSS(css)

end

local function setScrollPos(self, cur)
    self.cur_ = cur
    updateSliderV(self)
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

     updateSliderV(self)
end



function mod.onCreate(self)
    self.setScrollPos = setScrollPos
    self.setScrollRange = setScrollRange
    self.background_ = self:getChildAt(0)
    self.slider_ = self:getChildAt(1)
    self.min_ = 0
    self.max_ = 0
    self.view_ = 0
    self.cur_ = 0
end

function mod.onSizeChanged(self, w, h)
    local css = string.format("{width:%dpx; height:%dpx;}", w, h)
    self.background_:applyCSS(css)
    updateSliderV(self)
end


return mod