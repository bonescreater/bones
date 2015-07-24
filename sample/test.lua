
local mod = {}

function mod.fadeStart(ani, bone)
    print("start")
end

function mod.fadeStop(ani, bone)
    print("stop")
end

local function testFadeIn(btn)
    print("test fade in")
    fade = bones.getLuaObjectByID("test_fade")
    --animate.fadeIn
    fade:animate(90, 1800, "fadeIn", "animate", "fadeStop", "test", "fadeStart", "test");
end

local function testFadeOut(btn)
    print("test fade Out")
    fade = bones.getLuaObjectByID("test_fade")
    --animate.fadeOut
    fade:animate(90, 1800, "fadeOut", "animate", "fadeStop", "test", "fadeStart", "test");
end

local function testLayered(btn)
    panel = bones.getLuaObjectByID("p_layered")
    panel:applyCSS("{layered:true;}")
end

function mod.onPrepare(self)
print("test prepare")
btn1 = bones.getLuaObjectByID("fadein")
btn1:applyTextClass("fadein")
btn1["onClick"] = testFadeIn;
btn2 = bones.getLuaObjectByID("fadeout")
btn2:applyTextClass("fadeout")
btn2["onClick"] = testFadeOut;
btn3 = bones.getLuaObjectByID("close")
btn3:applyTextClass("close")
fade = bones.getLuaObjectByID("test_fade")

btn_layered = bones.getLuaObjectByID("layered")
btn_layered["onClick"] = testLayered;
btn_layered:applyTextCSS([[{content:"Layered窗口";}]])
end




return mod;