﻿local mod = {}

--setfenv(1, mod)

--control state
local common = 1
local highlight = 2
local press = 3
local disable = 4

local function stateChanged(self, force)
	state = common;
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
    local width, height = self:getSize();
    common_css = string.format("{rect:0px 0px;border:1px solid #ff999999;linear-gradient:0px 0px 0px %dpx clamp #ffffffff #ffd9d9d9;}", height);
    highlight_css = string.format("{rect:0px 0px;border:1px solid #ffaaaaaa;linear-gradient:0px 0px %dpx clamp #ffffffff #ffeaeaea;}", height);
    press_css = string.format("{rect:0px 0px;border:1px solid #ff999999;linear-gradient:0px 0px %dpx clamp #ffffffff #ffd3d3d3;}", height);
    disable_css = string.format("{rect:0px 0px;border:1px solid #ffbbbbbb;linear-gradient:0px 0px %dpx clamp #ffffffff #ffd9d9d9;}", height);

	if state ~= self.state_ or force then 
		self.state_ = state;
		if self.state_ == common then
			--self.shirt_:applyCSS("{linear-gradient:left top left bottom #ffffffff #ffd9d9d9;}")
			self.shape_:applyCSS(common_css)
		elseif self.state_ == highlight then
			--self.shirt_:applyCSS("{linear-gradient:left top left bottom #ffffffff #ffeaeaea;}")
			self.shape_:applyCSS(highlight_css)
		elseif self.state_ == press then
			--self.shirt_:applyCSS("{linear-gradient:left top left bottom #ffffffff #ffd3d3d3;}")
			self.shape_:applyCSS(press_css)
		elseif self.state_ == disable then
			--self.shirt_:applyCSS("{linear-gradient:left top left bottom #ffffffff #ffd9d9d9;}")
			self.shape_:applyCSS(disable_css)
		end
	end
	
end

--button
local function isEnable(self)
	return self.enable_
end

local function applyTextClass(self, class, mod)
    return self.text_:applyClass(class, mod)
end

local function applyTextCSS(self, css)
    return self.text_:applyCSS(css)
end

function mod.onPrepare(self)
print("Prepare")
print(self)
--public method
self.isEnable = isEnable
self.applyTextClass = applyTextClass
self.applyTextCSS = applyTextCSS
--private member
self.enable_ = true
self.hover_ = false
self.down_ = false

self.state_ = disable
self.shirt_ = self:getChildAt(0);
self.shape_ = self:getChildAt(1);
self.text_ = self:getChildAt(2);

end

function mod.onMouseEnter(self, e)
self:applyCSS("{cursor:hand;}")
end

function mod.onMouseMove(self, e)
	self.hover_ = self:contains(e:getLoc())
	stateChanged(self);
end

function mod.onMouseDown(self, e)
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
        	if type(self.onClick) == "function" then
					self:onClick();
            end
        end
	end
end

function mod.onMouseLeave(self, e)
	print("mouse leave")
	self.hover_ = false
	self.down_ = false
	stateChanged(self)
    self:applyCSS("{cursor:arrow;}")
end

function mod.onBlur(self, e)
end

function mod.onFocus(self, e)
end

function mod.onSizeChanged(self)
	local width, height = self:getSize();
	css = string.format("{width:%dpx;height:%dpx;}", width, height);
	self.shirt_:applyCSS(css);
	self.text_:applyCSS(css);
	self.shape_:applyCSS(css);

    stateChanged(self, true);
end






print("load button")
return mod