rotSpeed = 150.0
angle = 45.0
remainingAngle = 0.0
offset = float3.new(0, 240, -270)
scrollspeed = 15.0
targetAngle = 0.0
newZoomedPos = float3.new(0, 0, 0)
zoomSpeed = 50
function Update(dt)
    id = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
    if (id == 1) then
        target = Find("Zhib"):GetTransform():GetPosition()
    elseif (id == 2) then
        target = Find("Nerala"):GetTransform():GetPosition()
    elseif (id == 3) then
        target = Find("Omozra"):GetTransform():GetPosition()
    else
        target = 0
    end

    if (GetMouseZ() > 0) then
        newZoomedPos.x = newZoomedPos.x + gameObject:GetCamera():GetFront().x * zoomSpeed
        newZoomedPos.y = newZoomedPos.y + gameObject:GetCamera():GetFront().y * zoomSpeed
        newZoomedPos.z = newZoomedPos.z + gameObject:GetCamera():GetFront().z * zoomSpeed
    elseif (GetMouseZ() < 0) then
        newZoomedPos.x = newZoomedPos.x - gameObject:GetCamera():GetFront().x * zoomSpeed
        newZoomedPos.y = newZoomedPos.y - gameObject:GetCamera():GetFront().y * zoomSpeed
        newZoomedPos.z = newZoomedPos.z - gameObject:GetCamera():GetFront().z * zoomSpeed
    end

    if (GetInput(14) == KEY_STATE.KEY_DOWN) then -- Q
        remainingAngle = remainingAngle - angle
    end
    if (GetInput(15) == KEY_STATE.KEY_DOWN) then -- E
        remainingAngle = remainingAngle + angle
    end

    local newRemainingAngle = MoveTowards(remainingAngle, 0, rotSpeed * dt)
    local delta = remainingAngle - newRemainingAngle
    remainingAngle = newRemainingAngle
    local newQuat = Quat.new(float3.new(0, 1, 0), delta * 0.0174533)
    offset = MulQuat(newQuat, offset)
    local newPos = float3.new(0, 0, 0)
    newPos.x = target.x + offset.x
    newPos.y = target.y + offset.y
    newPos.z = target.z + offset.z
    if delta == 0 then
        local finalPos = float3.new(0, 0, 0)
        finalPos.x = newPos.x + newZoomedPos.x
        finalPos.y = newPos.y + newZoomedPos.y
        finalPos.z = newPos.z + newZoomedPos.z
        componentTransform:SetPosition(finalPos)
    else
        componentTransform:SetPosition(newPos)
    end

    gameObject:GetCamera():LookAt(target)
end

function MoveTowards(current, target, maxDistanceDelta)
    local delta = target - current
    local sqrDelta = delta * delta

    local sqrDistance = maxDistanceDelta * maxDistanceDelta

    if sqrDelta > sqrDistance then
        local magnitude = math.sqrt(sqrDelta)
        if magnitude > 0.000006 then
            delta = delta * (maxDistanceDelta / magnitude)
            delta = delta + current
            return delta
        else
            return current
        end
    end
    return target
end

