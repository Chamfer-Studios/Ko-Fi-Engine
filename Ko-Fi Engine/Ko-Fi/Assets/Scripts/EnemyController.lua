static = false
local staticIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL
staticIV = InspectorVariable.new("static", staticIVT, static)
NewVariable(staticIV)

speed = 20
local speedIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
speedIV = InspectorVariable.new("speed", speedIVT, speed)
NewVariable(speedIV)

player = nil
local playerIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_GAMEOBJECT
playerIV = InspectorVariable.new("player", playerIVT, playerName)
NewVariable(playerIV)

visionConeAngle = 90
local visionConeAngleIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
visionConeAngleIV = InspectorVariable.new("visionConeAngle", visionConeAngleIVT, visionConeAngle)
NewVariable(visionConeAngleIV)

visionConeRadius = 50
local visionConeRadiusIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
visionConeRadiusIV = InspectorVariable.new("visionConeRadius", visionConeRadiusIVT, visionConeRadius)
NewVariable(visionConeRadiusIV)

pingpong = false
local pingpongIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL
pingpongIV = InspectorVariable.new("pingpong", pingpongIVT, pingpong)
NewVariable(pingpongIV)

loop = false
local loopIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL
loopIV = InspectorVariable.new("loop", loopIVT, loop)
NewVariable(loopIV)

patrolOldWaypoints = {}
patrolWaypoints = {}
local patrolWaypointsIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT3_ARRAY
patrolWaypointsIV = InspectorVariable.new("patrolWaypoints", patrolWaypointsIVT, patrolWaypoints)
NewVariable(patrolWaypointsIV)

function Float3Length(v)
    return math.sqrt(v.x * v.x + v.y * v.y + v.z * v.z)
end

function Float3Difference(a, b)
    return { x = b.x - a.x, y = b.y - a.y, z = b.z - a.z }
end

function Float3Distance(a, b)
    diff = Float3Difference(a, b)
    return Float3Length(diff)
end

function Float3Dot(a, b)
    return a.x * b.x + a.y * b.y + a.z * b.z
end

function Float3Angle(a, b)
    lenA = Float3Length(a)
    lenB = Float3Length(b)

    return math.acos(Float3Dot(a, b) / (lenA * lenB))
end

function CheckAndRecalculatePath(force)
    -- We check whether the waypoint list has been updated, if it has, we recalculate the path.

    eq = true

    if #patrolWaypoints ~= #patrolOldWaypoints then
        eq = false
    end

    if eq == false then
        for i=1,#patrolWaypoints do
            for j=1,3 do
                patrolOldWaypoints[i] = {}
                patrolOldWaypoints[i][j] = patrolWaypoints[i][j]
            end
        end
    else
        for i=1,#patrolWaypoints do
            if patrolWaypoints[i].x ~= patrolOldWaypoints[i].x then
                eq = false
                patrolOldWaypoints[i].x = patrolWaypoints[i].x
            end
            if patrolWaypoints[i].y ~= patrolOldWaypoints[i].y then
                eq = false
                patrolOldWaypoints[i].y = patrolWaypoints[i].y
            end
            if patrolWaypoints[i].z ~= patrolOldWaypoints[i].z then
                eq = false
                patrolOldWaypoints[i].z = patrolWaypoints[i].z
            end
        end
    end

    if eq == false or force then
        DispatchEvent(pathfinderUpdateKey, { patrolWaypoints, pingpong, componentTransform:GetPosition() })
        currentPathIndex = 1
    end
end

pathfinderUpdateKey = "Pathfinder_UpdatePath"
pathfinderFollowKey = "Pathfinder_FollowPath"

STATE = {
    UNAWARE = 1,
    AWARE = 2,
    SUS = 3,
    AGGRO = 4,
    DEAD = 5,
    VICTORY = 6
}

state = STATE.UNAWARE

function LookAtDirection(direction)
    local a = Float3Angle(direction, float3.new(0, 0, 1))

    if direction.x < 0 then
        a = -a
    end

    componentTransform:SetRotation(float3.new(componentTransform:GetRotation().x, math.deg(a), componentTransform:GetRotation().z))
end

awareness = 0
targetAwareness = 0
awarenessSpeed = 0.1
hearingRange = 30
awarenessSource = nil
awarenessPosition = nil

function CheckAuditoryTriggerInRange(position, range)
    mypos = componentTransform:GetPosition()

    distance = Float3Distance(mypos, position)

    if distance < hearingRange + range then
        do return(true) end
    end

    do return(false) end
end

function ProcessSingleAuditoryTrigger(position, source)
    if state == STATE.UNAWARE then
        DispatchEvent("State_Suspicious", { position })
        do return end
    end

    if state == STATE.SUS then
        DispatchEvent("State_Aggressive", { source })
        do return end
    end
end

hadRepeatedAuditoryTriggerLastFrame = false
auditoryTriggerIsRepeating = false

function ProcessRepeatedAuditoryTrigger(position, source)
    if state == STATE.AGGRO then
        do return end
    end

    hadRepeatedAuditoryTriggerLastFrame = true
    if state == STATE.UNAWARE then
        SetTargetStateToSUS()
    elseif state == STATE.SUS then
        SetTargetStateToAGGRO()
    end
end

function ProcessAuditoryTrigger(position, range, type, source)
    if not CheckAuditoryTriggerInRange(position, range) then
        if auditoryTriggerIsRepeating == true then
            SetTargetStateToUNAWARE()
        end
        do return end
    end

    awarenessSource = source
    awarenessPosition = position

    if type == "single" then
        ProcessSingleAuditoryTrigger(position, source)
    elseif type == "repeated" then
        ProcessRepeatedAuditoryTrigger(position, source)
    end
end

function SetTargetStateToUNAWARE()
    if state == STATE.AGGRO then
        do return end
    end

    targetAwareness = 0
end

function SetTargetStateToSUS()
    if state == STATE.AGGRO then
        do return end
    end

    targetAwareness = 1
end

function SetTargetStateToAGGRO()
    targetAwareness = 2
end

function SetStateToUNAWARE()
    if state == STATE.AGGRO then
        do return end
    end
    
    CheckAndRecalculatePath(true)

    state = STATE.UNAWARE
    awareness = 0
    targetAwareness = 0
end

function SetStateToSUS(position)
    if state == STATE.AGGRO then
        do return end
    end

    if static == true then
        DispatchEvent(pathfinderUpdateKey, { {}, false, componentTransform:GetPosition() })
        LookAtDirection(Float3Difference(componentTransform:GetPosition(), position))
    else
        DispatchEvent(pathfinderUpdateKey, { { position }, false, componentTransform:GetPosition() })
    end

    state = STATE.SUS
    awareness = 1
    targetAwareness = 0
end

function SetStateToAGGRO(source)
    state = STATE.AGGRO
    awareness = 2
    targetAwareness = 2
end

function EventHandler(key, fields)
    if key == "Auditory_Trigger" then -- fields[1] -> position; fields[2] -> range; fields[3] -> type ("single", "repeated"); fields[4] -> source ("GameObject");
        ProcessAuditoryTrigger(fields[1], fields[2], fields[3], fields[4])
    elseif key == "State_Suspicious" then
        SetStateToSUS(fields[1])
    elseif key == "State_Aggressive" then
        SetStateToAGGRO(fields[1])
    elseif key == "Walking_Direction" then
        LookAtDirection(fields[1])
    end
end

function Start()
    CheckAndRecalculatePath(true)
end

function Update(dt)
    if awareness < targetAwareness then
        awareness = awareness + awarenessSpeed * dt
    elseif awareness > targetAwareness then
        awareness = awareness - awarenessSpeed * dt
    end

    Log(tostring(awareness) .. "\n")
    if awareness < 1.1 and awareness > 0.9 and state ~= STATE.SUS then
        DispatchEvent("State_Suspicious", { awarenessPosition })
    end

    if awareness < 0 then
        SetStateToUNAWARE()
    elseif awareness > 2 then
        DispatchEvent("State_Aggressive", { awarenessSource })
    end

    if state == STATE.UNAWARE then
        CheckAndRecalculatePath(false)
    end

    if hadRepeatedAuditoryTriggerLastFrame == true then
        auditoryTriggerIsRepeating = true
        hadRepeatedAuditoryTriggerLastFrame = false
    else
        if auditoryTriggerIsRepeating == true then
            auditoryTriggerIsRepeating = false
            SetTargetStateToUNAWARE()
        end
    end

    _loop = loop
    if state == STATE.SUS or state == STATE.AGGRO then
        _loop = false
    end
	DispatchEvent(pathfinderFollowKey, { speed, dt, _loop })
end