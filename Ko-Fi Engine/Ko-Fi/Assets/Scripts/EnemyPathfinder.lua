State = {
    PATROL = 1,
    SEEK = 2
}

currentState = State.PATROL
speed = 20
player = nil
lastPlayerPosition = nil
minRetargetingDistance = 3
visionConeAngle = 90
visionConeRadius = 50

local speedIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
speedIV = InspectorVariable.new("speed", speedIVT, speed)
NewVariable(speedIV)

local playerIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_GAMEOBJECT
playerIV = InspectorVariable.new("player", playerIVT, playerName)
NewVariable(playerIV)

local visionConeAngleIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
visionConeAngleIV = InspectorVariable.new("visionConeAngle", visionConeAngleIVT, visionConeAngle)
NewVariable(visionConeAngleIV)

local visionConeRadiusIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
visionConeRadiusIV = InspectorVariable.new("visionConeRadius", visionConeRadiusIVT, visionConeRadius)
NewVariable(visionConeRadiusIV)

local minRetargetingDistanceIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
minRetargetingDistanceIV = InspectorVariable.new("minRetargetingDistance", minRetargetingDistanceIVT, minRetargetingDistance)
NewVariable(minRetargetingDistanceIV)

oldWaypoints = {}
waypoints = {}

local waypointsIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT3_ARRAY
waypointsIV = InspectorVariable.new("waypoints", waypointsIVT, waypoints)
NewVariable(waypointsIV)

navigation = GetNavigation()

finalPath = {}
currentPathIndex = 1

forward = nil

function Float3Length(v)
    return math.sqrt(v.x * v.x + v.y * v.y + v.z * v.z)
end

function Float3Normalized(v)
    len = Float3Length(v)
    return { x = v.x / len, y = v.y / len, z = v.z / len }
end

function Float3Difference(a, b)
    return { x = b.x - a.x, y = b.y - a.y, z = b.z - a.z }
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

function FollowPath(dt)
    currentTarget = finalPath[currentPathIndex]
    currentPosition = componentTransform:GetPosition()
    if Float3Distance(currentTarget, currentPosition) <= minRetargetingDistance then
        currentPathIndex = currentPathIndex + 1
        if currentPathIndex > #finalPath then
            if currentState == State.PATROL then
                currentPathIndex = 1
            else
                currentPathIndex = #finalPath
            end
        end
        currentTarget = finalPath[currentPathIndex]
    end
    direction = Float3NormalizedDifference(currentPosition, currentTarget)
    forward = direction
    componentTransform:SetFront(float3.new(direction.x, direction.y, direction.z))
    delta = { x = direction.x * speed * dt, y = direction.y * speed * dt, z = direction.z * speed * dt }
    nextPosition = { x = currentPosition.x + delta.x, y = currentPosition.y + delta.y, z = currentPosition.z + delta.z }
    componentTransform:SetPosition(float3.new(nextPosition.x, nextPosition.y, nextPosition.z))
end

function CalculateFinalPath(wp)
    finalPath = {}
    playerPosition = componentTransform:GetPosition()
    --finalPath[1] = playerPosition
    n = 1
    for i=1,#wp - 1 do
        current = wp[i]
        next = wp[i + 1]
        result = navigation:FindPath(current, next, 1000, 1000)
        for j=1,#result do
            finalPath[n] = result[j]
            n = n + 1
        end
    end
end

function CheckAndRecalculatePath(force)
    -- We check whether the waypoint list has been updated, if it has, we recalculate the path.

    eq = true

    if #waypoints ~= #oldWaypoints then
        eq = false
    end

    if eq == false then
        for i=1,#waypoints do
            for j=1,3 do
                oldWaypoints[i] = {}
                oldWaypoints[i][j] = waypoints[i][j]
            end
        end
    else
        for i=1,#waypoints do
            if waypoints[i].x ~= oldWaypoints[i].x then
                eq = false
                oldWaypoints[i].x = waypoints[i].x
            end
            if waypoints[i].y ~= oldWaypoints[i].y then
                eq = false
                oldWaypoints[i].y = waypoints[i].y
            end
            if waypoints[i].z ~= oldWaypoints[i].z then
                eq = false
                oldWaypoints[i].z = waypoints[i].z
            end
        end
    end

    if eq == false or force then
        CalculateFinalPath(waypoints)
        currentPathIndex = 1
    end
end

function CheckAndRecalculateSeekTarget(force)
    if player == nil then
        return
    end

    playerPosition = player:GetTransform():GetPosition()

    if lastPlayerPosition == nil or Float3Distance(playerPosition, lastPlayerPosition) > minRetargetingDistance or force then
        lastPlayerPosition = float3.new(playerPosition.x, playerPosition.y, playerPosition.z)
        CalculateFinalPath({ componentTransform:GetPosition(), lastPlayerPosition })
        currentPathIndex = 1
    end
end

function ShouldSeekPlayer()
    position = componentTransform:GetPosition()
    --up = componentTransform:GetUp()

    --DrawCone(position, forward, up, visionConeAngle, visionConeRadius)

    if player == nil then
        return false
    end

    playerPosition = player:GetTransform():GetPosition()

    if Float3Distance(playerPosition, componentTransform:GetPosition()) > visionConeRadius then
        return false
    end
    
    direction = Float3NormalizedDifference(componentTransform:GetPosition(), playerPosition)

    angle = math.deg(Float3Angle(forward, direction))

    if angle > 180 then
        angle = angle - 360
    end

    if ((angle < visionConeAngle / 2) and (angle > -visionConeAngle / 2)) then
        return true
    end

    return false
end

hasSwitchedState = false

function Update(dt)
    playerPosition = player:GetTransform():GetPosition()
    if ShouldSeekPlayer() then
        if currentState ~= State.SEEK then
            hasSwitchedState = true
        end
        currentState = State.SEEK
    else
        if currentState ~= State.PATROL then
            hasSwitchedState = true
        end
        currentState = State.PATROL
    end

    if currentState == State.PATROL then
        CheckAndRecalculatePath(hasSwitchedState)
    else
        CheckAndRecalculateSeekTarget(hasSwitchedState)
    end

    hasSwitchedState = false

    if #finalPath ~= 0 then
        FollowPath(dt)
    end
end