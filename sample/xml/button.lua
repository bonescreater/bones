
local mod = {}

--control state
local common = 1
local highlight = 2
local press = 3
local disable = 4

--button

local function stateChanged(self)
	local state = common;
	if self:isEnable() ~= true then
		state = disable;
	elseif self.down_ then
		if self.hover_ then
		state = press;
		else
		state = highlight;
		end
	elseif self.hover_ then
		state = highlight;
	end

	if state ~= self.state_ then 
        self.state_ = state;
        local key;
		if self.state_ == common then
			key = self.common_
		elseif self.state_ == highlight then
			key = self.highlight_
		elseif self.state_ == press then
			key = self.press_
		elseif self.state_ == disable then
			key = self.disable_
		end
        self:setContent(key)
        self.pixmap_ = bones.resource.getPixmap(key)
	end
	
end

local function isEnable(self)
	return self.enable_
end


local function setBitmap(self, common, highlight, press, disable)
    self.common_ = common
    self.highlight_ = highlight
    self.press_ = press
    self.disable_ = disable
    stateChanged(self)
end

function mod.onHitTest(self, x, y)
    if self.pixmap_ then
        return not bones.pixmap.isTransparent(self.pixmap_, x, y)
    end
    return true
end

function mod.onCreate(self)
--public method
self.isEnable = isEnable
self.setBitmap = setBitmap
--private member
self.enable_ = true
self.hover_ = false
self.down_ = false

self.state_ = disable
self.focus_ = self:getChildAt(0)
self.focus_:setRect();
self.focus_:setColor(0xff00ffff)
self.focus_:setOpacity(0)
self.focus_:setStroke(true)
--不接受焦点
self.focus_:setFocusable(false)
--不接受鼠标
self.focus_:setMouseable(false)
--stateChanged(self)
end

function mod.onMouseEnter(self, e)

end

function mod.onMouseMove(self, e)
	self.hover_ = self:contains(e:getLoc())
	stateChanged(self);
end

function mod.onMouseDown(self, e)
	--tab切换焦点时 点击按钮隐藏边框
	self.focus_:setOpacity(0)
	if self:isEnable() and e:isLeftMouse() then
		if not self.down_ then
			self.down_ = true
			stateChanged(self)
		end
	end
end

function mod.onMouseUp(self, e)
    local notify = false

	if self:isEnable() and e:isLeftMouse() then
		if self.down_ then
			self.down_ = false
			if self.hover_ then
                notify = true
			end
			stateChanged(self)
		end
        if notify then
            print("click")
        end
	end
end

function mod.onMouseLeave(self, e)
	self.hover_ = false
	self.down_ = false
	stateChanged(self)
end

function mod.onSizeChanged(self, w, h)
    self.focus_:setSize(w, h)
end

function mod.onFocus(self, e)
    if e:isTabTraversal() then
        self.focus_:setOpacity(1)
    end
end

function mod.onBlur(self, e)
self.focus_:setOpacity(0)
end

return mod;