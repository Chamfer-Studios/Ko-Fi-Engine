State = {
    PATROL = 1,
    SEEK = 2
}

currentState = State.PATROL
speed = 20

local speedIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
speedIV = InspectorVariable.new("speed", speedIVT, speed)
NewVariable(speedIV)

oldWaypoints = {}
waypoints = {}

local waypointsIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT3_ARRAY
waypointsIV = InspectorVariable.new("waypoints", waypointsIVT, waypoints)
NewVariable(waypointsIV)

navigation = GetNavigation()

finalPath = {}
currentPathIndex = 1

function CalculateFinalPath()
    finalPath = {}
    n = 0
    for i=1,#waypoints - 1 do
        current = waypoints[i]
        next = waypoints[i + 1]
        result = navigation:FindPath(current, next, 1000)
        for j=1,#result do
            finalPath[n] = result[j]
            n = n + 1
        end
    end
end

function CheckAndRecalculatePath()
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

    if eq == false then
        CalculateFinalPath()
        currentPathIndex = 1
    end
end

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

function FollowPath(dt)
    currentTarget = finalPath[currentPathIndex]
    currentPosition = componentTransform:GetPosition()
    if Float3Distance(currentTarget, currentPosition) <= 3 then
        currentPathIndex = currentPathIndex + 1
        if currentPathIndex > #finalPath then
            currentPathIndex = 1
        end
        currentTarget = finalPath[currentPathIndex]
    end
    direction = Float3NormalizedDifference(currentPosition, currentTarget)
    delta = { x = direction.x * speed * dt, y = direction.y * speed * dt, z = direction.z * speed * dt }
    nextPosition = { x = currentPosition.x + delta.x, y = currentPosition.y + delta.y, z = currentPosition.z + delta.z }
    componentTransform:SetPosition(float3.new(nextPosition.x, nextPosition.y, nextPosition.z))
end

function Update(dt)
    CheckAndRecalculatePath()

    if #finalPath ~= 0 then
        FollowPath(dt)
    end
end