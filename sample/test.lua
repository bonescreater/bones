
local mod = {}

function mod.fadeStart(ani, bone)
    print("start")
end

function mod.fadeStop(ani, bone)
    print("stop")
end

local function testFadeIn(btn)
    print("test fade in")
    local fade = bones.getLuaObjectByID("rich")
    --animate.fadeIn
    local m = fade:animate(90, 1800, animate.fadeIn, test.fadeStop, test.fadeStart);
    print (m)
end

local function testFadeOut(btn)
    print("test fade Out")
    local fade = bones.getLuaObjectByID("rich")
    --animate.fadeOut
    fade:animate(90, 1800, animate.fadeOut, test.fadeStop, test.fadeStart);
end

local function testLayered(btn)
    local panel = bones.getLuaObjectByID("p_layered")
    panel:applyCSS("{ex-style:app-window layered;}")
    panel:applyCSS("{color:#8800ff00;}")
    --collectgarbage("collect")
end

function mod.onPrepare(self)
print("test prepare")
local btn1 = bones.getLuaObjectByID("fadein")
btn1:applyTextClass("fadein")
btn1["onClick"] = testFadeIn;
local btn2 = bones.getLuaObjectByID("fadeout")
btn2:applyTextClass("fadeout")
btn2["onClick"] = testFadeOut;
local btn3 = bones.getLuaObjectByID("close")
btn3:applyTextClass("close")

local btn_layered = bones.getLuaObjectByID("layered")
btn_layered["onClick"] = testLayered;
btn_layered:applyTextCSS([[{content:"Layered窗口";}]])
end

function mod.onSizeChanged(self, w, h)
print(string.format("width:%d, height:%d", w, h))
end


return mod;