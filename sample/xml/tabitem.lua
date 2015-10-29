
local mod = {}


local common = 1
local highlight = 2
--选中状态
local selected = 3

local function stateChanged(self)

    local state = common

    if self.selected_  == true then
        state = selected
    elseif self.down_ or self.hover_ then
        state = highlight
    end
    if state ~= self.state_ then
        self.state_ = state
        if state == selected then       
            self:setColor(0xffebeffa)
            self.top_line_:setVisible(true)
            self.right_line_:setVisible(false)
            self.bottom_line_:setVisible(true)
            self.circle_:setVisible(true)
        elseif state == highlight then
            self:setColor(0xffd7dced)
            self.top_line_:setVisible(false)
            self.right_line_:setVisible(true)
            self.bottom_line_:setVisible(false)
            self.circle_:setVisible(false)
            
        elseif state == common then
            self:setColor(0)
            
            --上颜色
            self.top_line_:setVisible(false)
            --右
            self.right_line_:setVisible(true)
            --下隐藏
            self.bottom_line_:setVisible(false)
            --圆隐藏
            self.circle_:setVisible(false)
            --text不变
        end
    end
end

local function setText(self, text)
    self.text_:setContent(text)
end

local function setSelected(self, selected)
    self.selected_ = selected
    stateChanged(self)
end

local function isSelected(self)
    return self.selected_
end

--设置回调
local function setDelegate(self, func)
    self.onSelected_ = func
end

function mod.onCreate(self)
    --public method
    self.setText = setText
    self.setSelected = setSelected
    self.isSelected = isSelected
    self.setDelegate = setDelegate
    --缓存并设置变量 除了底色shape 其它都不接受鼠标事件和焦点事件
    self:setRect(0,0)
    self.top_line_ = self:getChildAt(0)
    self.top_line_:setMouseable(false)
    self.top_line_:setFocusable(false)
    self.top_line_:setColor(0xffffffff)

    self.bottom_line_ = self:getChildAt(1)
    self.bottom_line_:setMouseable(false)
    self.bottom_line_:setFocusable(false)
    self.bottom_line_:setColor(0xffd5dadb)

    self.right_line_ = self:getChildAt(2)
    self.right_line_:setMouseable(false)
    self.right_line_:setFocusable(false)
    self.right_line_:setColor(0xffd5dadb)
    --self.right_line_:setColor(0xff000000)

    self.circle_ = self:getChildAt(3)
    self.circle_:setMouseable(false)
    self.circle_:setFocusable(false)
    self.circle_:setCircle(26, 31, 2)
    self.circle_:setColor(0xff727274)

    self.text_ = self:getChildAt(4)
    self.text_:setMouseable(false)
    self.text_:setFocusable(false)
    self.text_:setAuto(2, false)
    self.text_:setFont("Microsoft YaHei", 14, false, false, false, false)
    self.text_:setColor(0xff727274)
    self.hover_ = false
    self.down_ = false

    stateChanged(self)
end

function mod.onSizeChanged(self, w, h)
    --设置子的位置和尺寸
    self.top_line_:setLoc(0, 0)
    self.top_line_:setSize(w, 1)
    self.top_line_:setLine(0, 0, w, 0)

    self.bottom_line_:setLoc(0, h - 1)
    self.bottom_line_:setSize(w, 1)
    self.bottom_line_:setLine(0, 0, w, 0)

    self.right_line_:setLoc(w -1, 0)
    self.right_line_:setSize(1, h)
    self.right_line_:setLine(0, 0, 0, h)
    --
    self.circle_:setSize(w, h)

    self.text_:setSize(66, h)
end

function mod.onMouseMove(self, e)
	self.hover_ = self:contains(e:getLoc())
	stateChanged(self);
end

function mod.onMouseDown(self, e)
	if e:isLeftMouse() then
		if not self.down_ then
			self.down_ = true
			stateChanged(self)
--            self.selected_ = true
--            stateChanged(self)
            --发送selected通知
            if not self.selected_ and type(self.onSelected_) == "function" then
                self.onSelected_(self)
            end
		end
	end
end

function mod.onMouseUp(self, e)
	if e:isLeftMouse() and self.down_ then
		self.down_ = false
		stateChanged(self)
	end
end

function mod.onMouseLeave(self, e)
	self.hover_ = false
	self.down_ = false
	stateChanged(self)
end

return mod;