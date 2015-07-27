
local mod = {}

function mod.fadeStart(ani, bone)
    print("start")
end

function mod.fadeStop(ani, bone)
    print("stop")
end

local function testFadeIn(btn)
    print("test fade in")
    local fade = bones.getLuaObjectByID("test_fade")
    --animate.fadeIn
    local m = fade:animate(90, 1800, "fadeIn", "animate", "fadeStop", "test", "fadeStart", "test");
    print (m)
end

local function testFadeOut(btn)
    print("test fade Out")
    local fade = bones.getLuaObjectByID("test_fade")
    --animate.fadeOut
    fade:animate(90, 1800, "fadeOut", "animate", "fadeStop", "test", "fadeStart", "test");
end

local function testLayered(btn)
    local panel = bones.getLuaObjectByID("p_layered")
    panel:applyCSS("{ex-style:app-window layered;}")
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




return mod;