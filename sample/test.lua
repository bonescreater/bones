
local mod = {}

--self代表root标签
function mod.onCreate(self)
self:setColor(0xffff0000)
--单色
local s0 = self:getChildAt(0)
s0:setLoc(20, 20)
s0:setSize(50, 50)
--rect
s0:setRect(0, 0, 0, 0, 50, 50)
s0:setStroke(true)
s0:setColor(0xff00ffff)

----circle
--local s1 = self:getChildAt(1)
--s1:setLoc(80, 20)
--s1:setSize(50, 50)
--s1:setCircle(25, 25, 25)
--s1:setStroke(false)
--s1:setColor(0xffffffff)
----line
--local s2 = self:getChildAt(2)
--s2:setLoc(140, 20)
--s2:setSize(50, 50)
--s2:setLine(0, 0, 25, 25)
--s2:setStroke(false)
--s2:setColor(0xffffffff)
----point
local s3 = self:getChildAt(3)
s3:setLoc(200, 20)
s3:setSize(50, 50)
s3:setPoint(25, 25)
s3:setStroke(false)
s3:setStrokeWidth(5)
s3:setColor(0xffffffff)
----oval
--local s4 = self:getChildAt(4)
--s4:setLoc(260, 20)
--s4:setSize(50, 150)
--s4:setOval()
--s4:setStroke(false)
--s4:setColor(0xffffffff)
----arc
--local s5 = self:getChildAt(5)
--s5:setLoc(310, 20)
--s5:setSize(50, 150)
--s5:setArc(270, 90, true)
--s5:setStroke(false)
--s5:setColor(0xffffffff)

--local s6 = self:getChildAt(6)
--s6:setLoc(370, 20)
--s6:setSize(50, 150)
--s6:setRect(5, 5, 0, 0, 50, 150)
--s6:setStroke(true)
--s6:setColor(0xffffffff)
--s6:setStrokeEffect(2, 2, 2, 0)

local s8 = self:getChildAt(3)
s8:setLoc(80, 180)
s8:setSize(50, 50)
s8:setCircle(25, 25, 25)
s8:setStroke(false)
local radial = bones.createRadialGradient(25, 25, 25, 0, 2, 0xff00ff00, 0xff00ffff, 0, 1)
s8:setColor(radial)
bones.destroyShader(radial)

end



return mod;