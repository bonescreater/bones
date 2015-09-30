
local mod = {}


function mod.onCreate(self)
    local w, h = bones.getPixmapSize("bg")
    self:setContent("bg")
    self:setStretch()
	self:setColorMatrix(2, 0, 0, 0, 0,   0, 1, 0, 0, 0,   0, 0, 1, 0, 0,   0, 0, 0, 1, 0)
	--self:setColorMatrix()
end


return mod;