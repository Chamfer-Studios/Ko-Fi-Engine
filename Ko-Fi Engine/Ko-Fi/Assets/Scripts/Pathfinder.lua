minRetargetingDistance = 10
local minRetargetingDistanceIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
minRetargetingDistanceIV = InspectorVariable.new("minRetargetingDistance", minRetargetingDistanceIVT,
    minRetargetingDistance)
NewVariable(minRetargetingDistanceIV)

standByTime = 0
local standByTimeIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
standByTimeIV = InspectorVariable.new("standByTime", standByTimeIVT, standByTime)
NewVariable(standByTimeIV)

navigation = GetNavigation()

_G.finalPath = {}
currentPathIndex = 1

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

function FollowPath(speed, dt, loop, useRB)
    if #_G.finalPath == 0 or currentPathIndex > #_G.finalPath then
        do
            DispatchEvent("IsWalking", {false})
            if (componentRigidBody ~= nil and useRB == true) then
                componentRigidBody:SetLinearVelocity(float3.new(0.0, 0.0, 0.0))
            end
            do
                return
            end
        end
    end

    currentTarget = _G.finalPath[currentPathIndex]
    currentPosition = componentTransform:GetPosition()
    if Float3Distance(currentTarget, currentPosition) <= minRetargetingDistance then
        currentPathIndex = currentPathIndex + 1
        DispatchEvent("Patrol_Point", {true})
        standByTimer = 0

        if currentPathIndex > #_G.finalPath and loop then
            standByTimer = nil
            DispatchEvent("Patrol_Point", {nil})
            currentPathIndex = 2
            currentTarget = _G.finalPath[currentPathIndex]
        end
        if currentPathIndex > #_G.finalPath and not loop then
            currentPathIndex = currentPathIndex - 1
            currentTarget = _G.finalPath[currentPathIndex]
        end
        currentTarget = _G.finalPath[currentPathIndex]
    end
    if Float3Distance(currentTarget, currentPosition) >= minRetargetingDistance then
        direction = Float3NormalizedDifference(currentPosition, currentTarget)
        DispatchEvent("Walking_Direction", {float3.new(direction.x, direction.y, direction.z)})
        DispatchEvent("IsWalking", {true})
        delta = {
            x = direction.x * speed * _dt,
            y = direction.y * speed * _dt,
            z = direction.z * speed * _dt
        }
        if (componentRigidBody ~= nil and useRB == true) then
            componentRigidBody:SetLinearVelocity(float3.new(delta.x, delta.y, delta.z))
        else
            delta = {
                x = componentTransform:GetFront().x * speed * _dt,
                y = componentTransform:GetFront().y * speed * _dt,
                z = componentTransform:GetFront().z * speed * _dt
            }
            nextPosition = {
                x = currentPosition.x + delta.x,
                y = currentPosition.y + delta.y,
                z = currentPosition.z + delta.z
            }
            componentTransform:SetPosition(float3.new(nextPosition.x, nextPosition.y, nextPosition.z))
        end
    elseif currentPathIndex < #_G.finalPath then
        currentPathIndex = currentPathIndex + 1
        DispatchEvent("IsWalking", {false})
    else
        currentPathIndex = 2
        DispatchEvent("IsWalking", {false})
    end
end

function UpdatePath(wp, pingpong, currentPos)
    _finalPath = {}
    n = 1

    if #wp > 0 then
        result = navigation:FindPath(currentPos, wp[1], 1000, 1000)
        for j = 1, #result do
            _finalPath[n] = result[j]
            n = n + 1
        end
    end

    for i = 1, #wp - 1 do
        current = wp[i]
        next = wp[i + 1]
        result = navigation:FindPath(current, next, 1000, 1000)
        for j = 1, #result do
            _finalPath[n] = result[j]
            n = n + 1
        end
    end

    if pingpong == true then
        for i = #wp, 2, -1 do
            current = wp[i]
            next = wp[i - 1]
            result = navigation:FindPath(current, next, 1000, 1000)
            for j = 1, #result do
                _finalPath[n] = result[j]
                n = n + 1
            end
        end
    end

    _G.finalPath = {}

    for i = 1, #_finalPath do
        _G.finalPath[i] = float3.new(_finalPath[i].x, _finalPath[i].y, _finalPath[i].z)
    end

    closestIndex = 1

    for i = 1, #_G.finalPath do
        p = _G.finalPath[i]

        if (Float3Distance(currentPos, p) < Float3Distance(currentPos, _G.finalPath[closestIndex])) then
            closestIndex = i
        end
    end

    if (#_finalPath == 0) then
        DispatchEvent("Stop_Movement", {})
    end
    currentPathIndex = closestIndex
end

function EventHandler(key, fields)
    if key == "Pathfinder_UpdatePath" then -- fields[1] -> waypoints; fields[2] -> pingpong; fields[3] -> currentPos
        UpdatePath(fields[1], fields[2], fields[3])
    elseif key == "Pathfinder_FollowPath" then -- fields[1] -> speed; fields[2] -> dt; fields[3] -> loop;
        useRB = true
        if #fields == 4 then
            useRB = fields[4]
        end
        FollowPath(fields[1], fields[2], fields[3], useRB)
    end
end

_dt = 0.016

function Start()
    componentRigidBody = gameObject:GetRigidBody()
end

function Update(dt)
    _dt = dt
    if (standByTimer ~= nil) then

        standByTimer = standByTimer + dt
        if (standByTimer >= standByTime) then
            standByTimer = nil
            DispatchEvent("Patrol_Point", {nil})
        end
    end
end
