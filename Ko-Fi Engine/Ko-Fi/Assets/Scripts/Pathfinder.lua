minRetargetingDistance = 3
local minRetargetingDistanceIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
minRetargetingDistanceIV = InspectorVariable.new("minRetargetingDistance", minRetargetingDistanceIVT, minRetargetingDistance)
NewVariable(minRetargetingDistanceIV)

navigation = GetNavigation()

finalPath = {}
currentPathIndex = 1

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

function FollowPath(speed, dt)
    Log(tostring(speed) .. " " .. tostring(dt) .. " " .. tostring(#finalPath) .. "\n")
    if #finalPath == 0 then
        return
    end
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
    componentTransform:SetFront(float3.new(direction.x, direction.y, direction.z))
    delta = { x = direction.x * speed * dt, y = direction.y * speed * dt, z = direction.z * speed * dt }
    nextPosition = { x = currentPosition.x + delta.x, y = currentPosition.y + delta.y, z = currentPosition.z + delta.z }
    componentTransform:SetPosition(float3.new(nextPosition.x, nextPosition.y, nextPosition.z))
end

function UpdatePath(wp)
    finalPath = {}
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

function EventHandler(key, fields)
    if key == "Pathfinder_UpdatePath" then -- fields[1] -> waypoints;
        UpdatePath(fields[1])
        currentPathIndex = 1
    elseif key == "Pathfinder_FollowPath" then -- fields[1] -> speed; fields[2] -> dt;
        FollowPath(fields[1], fields[2])
    end
end