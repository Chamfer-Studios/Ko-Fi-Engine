rotSpeed = 0.0
angle = 0.0
remainingAngle = 0.0
offset = float3.new(0,0,0)
scrollspeed = 0.0

function  Update(dt)
    id = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)

    if (id == 1) then
        target = Find("Zhib"):GetTransform():GetPosition()
    elseif (id == 2) then
        target = Find("Nerala"):GetTransform():GetPosition()
    else
        target = 0
    end
    
    if (GetInput(14) == KEY_STATE.KEY_DOWN) then -- H
        remainingAngle = remainingAngle + angle 
    end
    if (GetInput(15) == KEY_STATE.KEY_DOWN) then -- K
        remainingAngle = remainingAngle - angle
    end

    local newRemainingAngle = MoveTowards(remainingAngle,0,rotSpeed * dt)
    local delta = remainingAngle - newRemainingAngle
    remainingAngle = newRemainingAngle
    offset = Quat.AxisAngle(float3.up,delta) * offset;
    componentTransform.SetPosition(target+offset)
    componentTransform.LookAt(target)
end

function MoveTowards(current,target,maxDistanceDelta)
    local delta = target-current
    local sqrDelta = delta*delta
    local sqrDistance = maxDistanceDelta * maxDistanceDelta
    
    if sqrDelta > sqrDistance then
        local magnitude = sqrt(sqrDelta)
        if magnitude > 1e-6 then
            delta.x = delta.x * (maxDistanceDelta/magnitude)
            delta.y = delta.y * (maxDistanceDelta/magnitude)
            delta.y = delta.y * (maxDistanceDelta/magnitude)
            delta.x = delta.x  + current.x 
            delta.y = delta.y + current.y
            delta.z = delta.z + current.z
            return delta
        else
            return current
        end
    end
    return target
end

