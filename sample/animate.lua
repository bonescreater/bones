local mod = {}

--固定步长
function mod.calcTime(interval, step, start, stop)
    return (math.abs(stop - start)) / step * interval;
end

function mod.fadeIn(bone, ani, progress)
    local org_opacity = bone["__current_opacity__"]
    if org_opacity == nil then
        bone["__current_opacity__"] = bone:getOpacity()
        org_opacity = bone["__current_opacity__"];
    end
    
    new_opacity = org_opacity + (1.0 - org_opacity) * progress
    bone:applyCSS(string.format("{opacity: %f;}", new_opacity))
    if progress == 1 then
        bone["__current_opacity__"] = nil
    end
end

function mod.fadeOut(bone, ani, progress)
    local org_opacity = bone["__current_opacity__"]
    if org_opacity == nil then
        bone["__current_opacity__"] = bone:getOpacity()
        org_opacity = bone["__current_opacity__"];
    end
    
    new_opacity = (org_opacity) * (1 - progress)
    print(new_opacity)
    bone:applyCSS(string.format("{opacity: %f;}", new_opacity))
    if progress == 1 then
        bone["__current_opacity__"] = nil
    end
end

print("load animate")
return mod;