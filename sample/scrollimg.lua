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
    self.vbar_ = self:getChildAt(1)
    self.vbar_:setSlave(self.scroller_)
    self.vbar_:setStyle(false)
    self.hbar_ = self:getChildAt(2)
    self.hbar_:setSlave(self.scroller_)
    self.hbar_:setStyle(true)
end

function mod.onSizeChanged(self, w, h)
    local cw, ch = w - 12, h - 12
    local css = string.format("{width:%dpx; height:%dpx;}", cw, ch)
    self.scroller_:applyCSS(css)

    css = string.format("{width:%dpx; height:%dpx;}", 12, ch)
    self.vbar_:applyCSS(css)
    css = string.format("{left:%dpx; top:%dpx;}", cw, 0)
    self.vbar_:applyCSS(css)

    css = string.format("{width:%dpx; height:%dpx;}", cw, 12)
    self.hbar_:applyCSS(css)
    css = string.format("{left:%dpx; top:%dpx;}", 0, ch)
    self.hbar_:applyCSS(css)
end

--通知scrollbar
--self是 scroller标签 所以要通过父来找到scrollerbar
function mod.onScrollPos(self, cur, horiz)
    local parent = self:getParent()
    local vbar = parent.vbar_
    local hbar = parent.hbar_
    if horiz then
        hbar:setScrollPos(cur)
    else
        vbar:setScrollPos(cur)
    end
end

function mod.onScrollRange(self, min, max, view, horiz)
    local parent = self:getParent()
    local vbar = parent.vbar_
    local hbar = parent.hbar_
    --调用scrollbar的setScrollRange
    if horiz then
        hbar:setScrollRange(min, max, view)
    else
        vbar:setScrollRange(min, max, view)
    end
end


return mod