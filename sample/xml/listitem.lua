
local mod = {}
--136 - 84
--设置指定的行列元素
local function addItem(self, ln, col, common, highlight, press, disable)
    --按钮的高度就是一行的高度
    if ln < 1 then
        return
    end

    --填满items的nil
    for i = 1, ln do
        local item = self.items_[i]
        if not item then
            self.items_[i] = {}
        end
    end

    local btn_wh = 84
    local btn_wspace = 10
    local btn_hspace = 0
    local btn_t = 40
    local org_h = 52

    local item = bones.createObject(self, "button")
    item:setBitmap(common, highlight, press, disable)
    item:setSize(btn_wh, btn_wh)
    item:setLoc((col - 1) * btn_wh + btn_wspace * (col - 1),
                 btn_t + (ln - 1) * btn_wh + (ln - 1) * btn_hspace)
    
    
    self.items_[ln][col] = item
    --更新宽高
    local len = #self.items_
    local w, h = self:getSize()
    local new_height = len * btn_wh + btn_hspace * (len - 1) + org_h

    if h < new_height then
        h = new_height
    end
    self:setSize(w, h)
end

local function setText(self, text)
    self.text_:setContent(text)
end

function mod.onCreate(self)
    self.addItem = addItem
    self.setText = setText
    --self是线
    self:setColor(0xffd5dadb)

    self.text_ = self:getChildAt(0)
    self.text_:setFont("Microsoft YaHei", 14, false, false, false, false)
    self.text_:setColor(0xff727274)
    self.text_:setAuto(0, false)
    self.text_:setLoc(18, 0)
    self.text_:setSize(100, 56)
    self.items_ = {}
end

function mod.onSizeChanged(self, w, h)
    self:setLine(18, h - 1, w - 18, h - 1)
end

return mod;