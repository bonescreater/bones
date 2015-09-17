local mod = {}


local function setContent(self, content)

    local img = self.scroller_:getContentAt(0)    
    --设置img的宽高为指定图片宽高
    local css = string.format("{content:%s;}", content)
    img:applyCSS(css)
    w, h = bones.getPixmapSize(content)       
    css = string.format("{size:%dpx %dpx;}", w, h)
    img:applyCSS(css)

    --设置scrollimg可以滚动
    self.scroller_:setScrollInfo(w,true)
    self.scroller_:setScrollInfo(h, false)
end

function mod.onCreate(self)
    self.setContent = setContent
    self.scroller_ = self:getChildAt(0)
    self.bar_ = self:getChildAt(1)
end

function mod.onSizeChanged(self, w, h)
    local css = string.format("{width:%dpx; height:%dpx;}", w - 12, h)
    self.scroller_:applyCSS(css)
    css = string.format("{width:%dpx; height:%dpx;}", 12, h)
    self.bar_:applyCSS(css)
    css = string.format("{left:%dpx; top:%dpx;}", w - 12, 0)
    self.bar_:applyCSS(css)
end

--通知scrollbar
--self是 scroller标签 所以要通过父来找到scrollerbar
function mod.onScrollPos(self, cur, horiz)
    local parent = self:getParent()
    local bar = parent.bar_
    bar:setScrollPos(cur)
end

function mod.onScrollRange(self, min, max, view, horiz)
    local parent = self:getParent()
    local bar = parent.bar_
    --调用scrollbar的setScrollRange
    bar:setScrollRange(min, max, view)
end


return mod