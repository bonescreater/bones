local mod = {}


function mod.onCreate(self)
    self.scroller_ = self:getChildAt(0)
    self.bar_ = self:getChildAt(1)
end

function mod.onSizeChanged(self, w, h)
    local css = string.format("{width:%dpx; height:%dpx;}", w, h)
    self.scroller_:applyCSS(css)
end

--通知scrollbar
--self是 scroller标签 所以要通过父来找到scrollerbar
function mod.onScrollPos(self, cur, horiz)
    local parent = self:getParent()
    local bar = parent.bar_
    print(cur)
end

return mod