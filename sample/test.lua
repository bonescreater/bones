
local mod = {}

--self代表root标签
function mod.onCreate(self)
self:setColor(0xffff0000)

local s0 = self:getChildAt(0)
local t0 = s0:getChildAt(0)
s0:setSize(200, 200)
s0:setLoc(20, 20)
s0:setRect(0, 0)
s0:setStroke(true)
s0:setColor(0xff00ff00)

t0:setFont("Microsoft Yahei", 14, true, true, true, true)
t0:setContent("bones ui中文")
t0:setPos(10, 20, 20, 30, 30, 40, 40, 50, 50, 60, 60, 70, 70, 80, 80, 90, 90, 100, 100, 110, 110)
t0:setSize(198, 198)
t0:setLoc(1, 1)

local s1 = self:getChildAt(1)
local t1 = s1:getChildAt(0)
s1:setSize(200, 200)
s1:setLoc(230, 20)
s1:setRect(0, 0)
s1:setStroke(true)
s1:setColor(0xff00ff00)

t1:setFont("Microsoft Yahei", 14, false, false, true, true)
t1:setContent("bones ui\n中\n文")
t1:setAuto(0, 1, 0)
t1:setSize(198, 198)
t1:setLoc(1, 1)

local s2 = self:getChildAt(2)
local t2 = s2:getChildAt(0)
s2:setSize(200, 200)
s2:setLoc(440, 20)
s2:setRect(0, 0)
s2:setStroke(true)
s2:setColor(0xff00ff00)

t2:setFont("Microsoft Yahei", 14, false, false, true, true)
t2:setContent("bones ui\n中\n文")
t2:setAuto(2, 1, 0)
t2:setSize(198, 198)
t2:setLoc(1, 1)

local s3 = self:getChildAt(3)
local t3 = s3:getChildAt(0)
s3:setSize(200, 200)
s3:setLoc(650, 20)
s3:setRect(0, 0)
s3:setStroke(true)
s3:setColor(0xff00ff00)

t3:setFont("Microsoft Yahei", 14, false, false, true, true)
t3:setContent("bones ui\n中\n文")
t3:setAuto(1, 1, 0)
t3:setSize(198, 198)
t3:setLoc(1, 1)

local s4 = self:getChildAt(4)
local t4 = s4:getChildAt(0)
s4:setSize(200, 200)
s4:setLoc(20, 230)
s4:setRect(0, 0)
s4:setStroke(true)
s4:setColor(0xff00ff00)

t4:setFont("Microsoft Yahei", 14, true, true, true, true)
t4:setContent("bones ui\n中文abcd大师法法大赛的发生打发似的")
t4:setAuto(1, 2, 10)
t4:setSize(198, 198)
t4:setLoc(1, 1)

local s5 = self:getChildAt(5)
local t5 = s5:getChildAt(0)
s5:setSize(200, 200)
s5:setLoc(230, 230)
s5:setRect(0, 0)
s5:setStroke(true)
s5:setColor(0xff00ff00)

t5:setFont("Microsoft Yahei", 14, true, true, true, true)
t5:setContent("bones ui\n中文abcd大师法法大赛的发生打发似的")
t5:setAuto(1, 1, 10)
t5:setSize(198, 198)
t5:setLoc(1, 1)

local shader = bones.shader.createLinearGradient(0, 0, 40, 0, 2, 3, 0xffff0000, 0xff00ff00, 0xff0000ff, 0, 0.5, 1);
t5:setColor(shader)
bones.shader.release(shader)

end





return mod;