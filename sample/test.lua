
local mod = {}

--control state
local common = 1
local highlight = 2
local press = 3
local disable = 4

--button
local function isEnable(self)
	return self.enable_
end

local function stateChanged(self)
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

	if state ~= self.state_ then 
		self.state_ = state;
		if self.state_ == common then
			self:setContent("common")
		elseif self.state_ == highlight then
			self:setContent("highlight")
		elseif self.state_ == press then
			self:setContent("press")
		elseif self.state_ == disable then
			self:setContent("disable")
		end
	end
	
end

function mod.onCreate(self)
--public method
self.isEnable = isEnable
--private member
self.enable_ = true
self.hover_ = false
self.down_ = false

self.state_ = disable
stateChanged(self)
end

function mod.onMouseEnter(self, e)

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
            print("click")
        end
	end
end

function mod.onMouseLeave(self, e)
	self.hover_ = false
	self.down_ = false
	stateChanged(self)
end



return mod;