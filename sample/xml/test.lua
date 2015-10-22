
local mod = {}

--self代表root标签
function mod.onCreate(self)
    self.image_ = self:getChildAt(0)
    self.scroller_ = self:getChildAt(1)

    --设置image标签显示完整的图片
    local pm_key = "lena"
    local pm = bones.resource.getPixmap(pm_key)
    local w, h = bones.pixmap.getSize(pm)
    print(w)
    self.image_:setSize(w, h)
    self.image_:setContent(pm_key)

    --关联scroller 和滚动条
    local vbar = self:getChildAt(2)
    local hbar = self:getChildAt(3)
    self.scroller_.vbar_ = vbar
    self.scroller_.hbar_ = hbar
    vbar:setSlave(self.scroller_)
    vbar:setStyle(false)
    hbar:setSlave(self.scroller_)
    hbar:setStyle(true)

    --设置scroller大小 即视口
    self.scroller_:setSize(w / 2, h / 2)
    self.scroller_:setLoc(w + 10, 10)
    --设置滚动条的位置 
    local bar_const = 12
    vbar:setLoc(w + 10 + w / 2, 10)
    vbar:setSize(bar_const, h / 2)
    --滚动条的位置可以是任意的 垂直滚动条是紧贴 水平滚动条可以离远点
    local delta = 20
    hbar:setLoc(w + 10, 10 + h /2 + delta )
    hbar:setSize(w / 2, bar_const)
    --需要滚动显示image标签大小为图片大小
    local image = self.scroller_:getChildAt(0)
    image:setSize(w, h)
    image:setContent(pm_key)

    --scroller计算滚动范围
    self.scroller_:setScrollInfo(h, false)
    self.scroller_:setScrollInfo(w, true)

end


function mod.onScrollRange(self, min, max, view, horiz)
    local vbar = self.vbar_
    local hbar = self.hbar_
    --调用scrollbar的setScrollRange
    if horiz then
        hbar:setScrollRange(min, max, view)
    else
        vbar:setScrollRange(min, max, view)
    end
end

function mod.onScrollPos(self, cur, horiz)
    local vbar = self.vbar_
    local hbar = self.hbar_
    if horiz then
        hbar:setScrollPos(cur)
    else
        vbar:setScrollPos(cur)
    end
end

return mod;