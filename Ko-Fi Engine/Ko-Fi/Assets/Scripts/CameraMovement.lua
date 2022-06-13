rotSpeed = 150.0
panSpeed = 150.0
zoomSpeed = 50
finalPos = float3.new(0, 0, 0)
angle = 45.0
remainingAngle = 0.0
offset = float3.new(0, 200, 270)
scrollspeed = 15.0
targetAngle = 0.0
newZoomedPos = float3.new(0, 0, 0)
mosquitoAlive = false
zPanning = 0.0 -- 1 to go front, -1 to go back
xPanning = 0.0 -- 1 to go right, -1 to go left

borderXNegative = -2000
borderXPositive = 2190
borderZNegative = -1900
borderZPositive = 1350

borderXNegative1 = -2000
borderXPositive1 = 2190
borderZNegative1 = -1900
borderZPositive1 = 1350

borderXNegative2 = -1900
borderXPositive2 = 1880
borderZNegative2 = -900
borderZPositive2 = 1075

cursorMargin = 25
mouseTopIn = true
mouseBottomIn = true
mouseLeftIn = true
mouseRightIn = true

camSensitivity = 0.2
lastDeltaX = 0
-- resetOffset = 1;
-- currentTarget = float3.new(0, 0, 0)
closestY = -100.0
furthestY = 2000.0
rayCastCulling = {}
local freePanningDebug
isStarting = true
-- use the fokin start

function Float3Length(v)
    return math.sqrt(v.x * v.x + v.y * v.y + v.z * v.z)
end

function Float3Normalized(v)
    len = Float3Length(v)
    return {
        x = v.x / len,
        y = v.y / len,
        z = v.z / len
    }
end

function Float3Difference(a, b)
    return {
        x = b.x - a.x,
        y = b.y - a.y,
        z = b.z - a.z
    }
end

function Float3Distance(a, b)
    diff = Float3Difference(a, b)
    return Float3Length(diff)
end

function Float3NormalizedDifference(a, b)
    v = Float3Difference(a, b)
    return Float3Normalized(v)
end

function Float3Dot(a, b)
    return a.x * b.x + a.y * b.y + a.z * b.z
end

function Float3Angle(a, b)
    lenA = Float3Length(a)
    lenB = Float3Length(b)

    return math.acos(Float3Dot(a, b) / (lenA * lenB))
end

function Float3Cross(a, b)
    return float3.new(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x)
end

function Float3Mult(a, b)
    return float3.new(a.x * b, a.y * b, a.z * b)
end

function Float3Sum(a, b)
    return float3.new(a.x + b.x, a.y + b.y, a.z + b.z)
end

function Float3RotateAxisAngle(v, axis, angle)
    do
        anglecos = math.cos(angle)
        vbycos = float3.new(v.x * anglecos, v.y * anglecos, v.z * anglecos)
        anglesin = math.sin(angle)
        axisvectorcross = Float3Cross(axis, v)
        axisvectordot = Float3Dot(axis, v)
        ret = Float3Sum(Float3Sum(vbycos, Float3Mult(axisvectorcross, anglesin)),
            (Float3Mult(axis, axisvectordot * (1 - anglecos))))

        do
            return ret
        end
    end
end

function Start()
    -- Put the position of the selected character inside variable target
    level = GetVariable("GameState.lua", "levelNumber", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
    if level == 1 then
        borderXNegative = borderXNegative1
        borderXPositive = borderXPositive1
        borderZNegative = borderZNegative1
        borderZPositive = borderZPositive1
        -- Log("Level 1")
    elseif level == 2 then
        borderXNegative = borderXNegative2
        borderXPositive = borderXPositive2
        borderZNegative = borderZNegative2
        borderZPositive = borderZPositive2
        -- Log("Level 2")
    end
    -- Log("Level xD")
    freePanningDebug = false
    GetSelectedCharacter()
    offset = float3.new(0, 240, 270)
    newZoomedPos = float3.new(0, 99, 112)

end

function Update(dt)

    if (GetMouseScreenPos().y < 0 or GetMouseScreenPos().y > GetLastViewportSize().y or GetMouseScreenPos().x < 0 or
        GetMouseScreenPos().x > GetLastViewportSize().x) then
        xPanning = 0.0
        zPanning = 0.0
    end
    -- str = "Position X: " .. tostring(gameObject:GetTransform():GetPosition().x) .. "Position Z: " .. tostring(gameObject:GetTransform():GetPosition().z) .. "\n"
    -- Log(str)
    local lastFinalPos = componentTransform:GetPosition()
    -- input: mouse wheel to zoom in and out
    -- local?
    if (GetMouseZ() > 0) then
        local deltaY = newZoomedPos.y + gameObject:GetCamera():GetFront().y * zoomSpeed
        if math.abs(deltaY) < 110 then
            newZoomedPos.y = newZoomedPos.y + gameObject:GetCamera():GetFront().y * zoomSpeed
            newZoomedPos.x = newZoomedPos.x + gameObject:GetCamera():GetFront().x * zoomSpeed
            newZoomedPos.z = newZoomedPos.z + gameObject:GetCamera():GetFront().z * zoomSpeed
        end
    elseif (GetMouseZ() < 0) then
        local deltaY = newZoomedPos.y - gameObject:GetCamera():GetFront().y * zoomSpeed
        if math.abs(deltaY) < 110 then
            newZoomedPos.y = newZoomedPos.y - gameObject:GetCamera():GetFront().y * zoomSpeed
            newZoomedPos.x = newZoomedPos.x - gameObject:GetCamera():GetFront().x * zoomSpeed
            newZoomedPos.z = newZoomedPos.z - gameObject:GetCamera():GetFront().z * zoomSpeed
        end
    end

    if (GetInput(16) == KEY_STATE.KEY_REPEAT) then -- W --HAY UN KEY REPEAT
        zPanning = -1.0
    end
    if (GetInput(16) == KEY_STATE.KEY_UP) then -- W
        zPanning = 0.0
    end
    if (GetInput(17) == KEY_STATE.KEY_REPEAT) then -- A
        xPanning = -1
    end
    if (GetInput(17) == KEY_STATE.KEY_UP) then -- A
        xPanning = 0.0
    end
    if (GetInput(18) == KEY_STATE.KEY_REPEAT) then -- S
        zPanning = 1.0
    end
    if (GetInput(18) == KEY_STATE.KEY_UP) then -- S
        zPanning = 0.0
    end
    if (GetInput(19) == KEY_STATE.KEY_REPEAT) then -- D
        xPanning = 1.0
    end
    if (GetInput(19) == KEY_STATE.KEY_UP) then -- D
        xPanning = 0.0
    end

    -- Mouse Movement

    if (GetMouseScreenPos().y < GetLastViewportSize().y and GetMouseScreenPos().y >
        (GetLastViewportSize().y - cursorMargin)) then -- W --HAY UN KEY REPEAT
        mouseTopIn = true
        zPanning = -1.0
    end
    if ((GetMouseScreenPos().y < (GetLastViewportSize().y) - cursorMargin and GetMouseScreenPos().y > cursorMargin) and
        mouseTopIn == true) then -- W
        mouseTopIn = false
        zPanning = 0.0
    end
    if ((GetMouseScreenPos().x > 0 and GetMouseScreenPos().x < cursorMargin)) then -- A
        mouseLeftIn = true
        xPanning = -1.0
    end
    if (GetMouseScreenPos().x > cursorMargin and GetMouseScreenPos().x < (GetLastViewportSize().x - cursorMargin) and
        mouseLeftIn == true) then -- A
        mouseLeftIn = false
        xPanning = 0.0
    end
    if (GetMouseScreenPos().y > 0 and GetMouseScreenPos().y < cursorMargin) then -- S
        mouseBottomIn = true
        zPanning = 1.0
    end
    if (GetMouseScreenPos().y > cursorMargin and GetMouseScreenPos().y < (GetLastViewportSize().y - cursorMargin) and
        GetMouseScreenPos().y < 0 and mouseBottomIn == true) then -- S
        mouseBottomIn = false
        zPanning = 0.0
    end
    if (GetMouseScreenPos().x < GetLastViewportSize().x and GetMouseScreenPos().x >
        (GetLastViewportSize().x - cursorMargin)) then -- D
        mouseRightIn = true
        xPanning = 1.0
    end
    if (GetMouseScreenPos().x < (GetLastViewportSize().x) - cursorMargin and GetMouseScreenPos().x > 15 and mouseRightIn ==
        true) then -- D
        mouseRightIn = false
        xPanning = 0.0
    end
    -- go back to the selected character
    -- if  (GetInput(10) == KEY_STATE.KEY_DOWN) then -- R
    --     GetSelectedCharacter()
    --     offset = float3.new(0, 240, 270)
    -- end
    -- if  (GetInput(10) == KEY_STATE.KEY_DOWN) then -- H
    --     if freePanningDebug then
    --         freePanningDebug = false
    --     else
    --         freePanningDebug = true
    --     end

    -- end
    if (GetInput(43) == KEY_STATE.KEY_DOWN) then -- SPACE
        freePanningDebug = not freePanningDebug
        if freePanningDebug == true then
            GetSelectedCharacter()
            offset = float3.new(0, 240, 270)
            newZoomedPos = float3.new(0, 99, 112)
            CleanCulling()
        end
    end

    if (GetInput(14) == KEY_STATE.KEY_REPEAT) then -- Q
        local newQuat = Quat.new(float3.new(0, 1, 0), -0.0174533)
        offset = MulQuat(newQuat, offset)
        newZoomedPos = MulQuat(newQuat, newZoomedPos)
    end
    if (GetInput(15) == KEY_STATE.KEY_REPEAT) then -- E
        local newQuat = Quat.new(float3.new(0, 1, 0), 0.0174533)
        offset = MulQuat(newQuat, offset)
        newZoomedPos = MulQuat(newQuat, newZoomedPos)

    end

    if (GetMouseMotionX() > 0 and GetInput(2) == KEY_STATE.KEY_REPEAT) then
        xMotion = GetMouseMotionX()
        newDeltaX = xMotion * camSensitivity -- camera sensitivity
        deltaX = newDeltaX + 0.8 * (lastDeltaX - newDeltaX)
        lastDeltaX = deltaX
        finalDelta = deltaX * dt
        local newQuat = Quat.new(float3.new(0, 1, 0), finalDelta)
        offset = MulQuat(newQuat, offset)
        newZoomedPos = MulQuat(newQuat, newZoomedPos)
        xMotion = 0
    end

    if (GetMouseMotionX() < 0 and GetInput(2) == KEY_STATE.KEY_REPEAT) then
        xMotion = GetMouseMotionX()
        newDeltaX = xMotion * camSensitivity -- camera sensitivity
        deltaX = newDeltaX + 0.8 * (lastDeltaX - newDeltaX)
        lastDeltaX = deltaX
        finalDelta = deltaX * dt
        local newQuat = Quat.new(float3.new(0, 1, 0), finalDelta)
        offset = MulQuat(newQuat, offset)
        newZoomedPos = MulQuat(newQuat, newZoomedPos)
        xMotion = 0
    end

    -- modify target with the camera panning
    if (freePanningDebug == true) then
        local currentPanSpeed = panSpeed * dt
        panning = float3.new(xPanning, 0, zPanning)

        front = componentTransform:GetGlobalFront()
        front.y = 0
        front = Float3Normalized(front)

        right = componentTransform:GetGlobalRight()
        right.y = 0
        right = Float3Normalized(right)

        panning = Float3Sum(Float3Mult(right, panning.x), Float3Mult(front, panning.z))

        -- str = "Front Normalized: " .. tostring(panning) .. "\n"
        -- Log(str)

        if (target.z >= borderZPositive and panning.z > 0) then
            panning = float3.new(0, 0, 0)
        end
        if (target.z <= borderZNegative and panning.z < 0) then
            panning = float3.new(0, 0, 0)
        end
        if (target.x <= borderXNegative and panning.x < 0) then
            panning = float3.new(0, 0, 0)
        end
        if (target.x >= borderXPositive and panning.x > 0) then
            panning = float3.new(0, 0, 0)
        end

        target.z = target.z + (panning.z * currentPanSpeed)
        target.x = target.x + (panning.x * currentPanSpeed)
    else
        GetSelectedCharacter()
    end

    -- Log("current target " .. target.x .. " " .. target.z .. "\n")

    -- add offset to the target to set the current camera position
    local newPos = float3.new(0, 0, 0)
    newPos.x = target.x + offset.x
    newPos.y = target.y + offset.y
    newPos.z = target.z + offset.z

    -- --compute final position adding zoom
    finalPos.x = newPos.x + newZoomedPos.x
    finalPos.y = newPos.y + newZoomedPos.y
    finalPos.z = newPos.z + newZoomedPos.z

    componentTransform:SetPosition(finalPos)

    gameObject:GetCamera():LookAt(target)

    if (freePanningDebug == false) then
        if componentTransform:GetPosition() ~= lastFinalPos then
            for i = 1, #rayCastCulling do
                rayCastCulling[i].active = true
            end
            if #rayCastCulling > 0 then
                rayCastCulling = {}
            end
            rayCastCulling = CustomRayCastList(finalPos, target, {Tag.UNTAGGED, Tag.WALL, Tag.DECORATIONFLOOR})
            for j = 1, #rayCastCulling do
                rayCastCulling[j].active = false
            end
        end
        -- 1st iteration use look at to center at characters

    end
end

function EventHandler(key, fields) -- funcion virtual que recibe todos los eventos que se componen de una key y unos fields. la cosa esta en especificar la key (quees el evento) 
    if (key == "Mosquito_Spawn") then
        mosquito = fields[1]
    elseif (key == "Mosquito_Death") then
        mosquito = nil
    end
end

function GetSelectedCharacter()

    -- get character selected. I keep it in order to center the camera arround a player in case needed
    id = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
    if (id == 1) then
        target = Find("Zhib"):GetTransform():GetPosition()
        lastTarget = id
    elseif (id == 2) then
        if (mosquito ~= nil) then
            target = mosquito:GetTransform():GetPosition()
        else
            target = Find("Nerala"):GetTransform():GetPosition()
        end
        lastTarget = id
    elseif (id == 3) then
        target = Find("Omozra"):GetTransform():GetPosition()
        lastTarget = id
    else
        if (lastTarget == 1) then
            target = Find("Zhib"):GetTransform():GetPosition()
        elseif (lastTarget == 2) then
            if (mosquitoAlive == true and mosquito ~= nil) then
                target = mosquito:GetTransform():GetPosition()
            else
                target = Find("Nerala"):GetTransform():GetPosition()
            end
        elseif (lastTarget == 3) then
            target = Find("Omozra"):GetTransform():GetPosition()
        end
    end

end

function CleanCulling() 

    for j = 1, #rayCastCulling do
        rayCastCulling[j].active = true
    end

    rayCastCulling = {}
end