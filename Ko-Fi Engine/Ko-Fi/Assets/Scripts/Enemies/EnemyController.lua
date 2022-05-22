static = false
local staticIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL
staticIV = InspectorVariable.new("static", staticIVT, static)
NewVariable(staticIV)

speed = 2000
local speedIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
speedIV = InspectorVariable.new("speed", speedIVT, speed)
NewVariable(speedIV)

chaseSpeed = 3000
local chaseSpeedIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
chaseSpeedIV = InspectorVariable.new("chaseSpeed", chaseSpeedIVT, chaseSpeed)
NewVariable(chaseSpeedIV)

visionConeAngle = 90
local visionConeAngleIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
visionConeAngleIV = InspectorVariable.new("visionConeAngle", visionConeAngleIVT, visionConeAngle)
NewVariable(visionConeAngleIV)

visionConeRadius = 50
local visionConeRadiusIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
visionConeRadiusIV = InspectorVariable.new("visionConeRadius", visionConeRadiusIVT, visionConeRadius)
NewVariable(visionConeRadiusIV)

hearingRange = 30
local hearingRangeIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
hearingRangeIV = InspectorVariable.new("hearingRange", hearingRangeIVT, hearingRange)
NewVariable(hearingRangeIV)

awarenessOffset = float3.new(0, 50, 0)
local awarenessOffsetIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT3
awarenessOffsetIV = InspectorVariable.new("awarenessOffset", awarenessOffsetIVT, awarenessOffset)
NewVariable(awarenessOffsetIV)

awarenessSize = float3.new(0.15, 0.3, 0.15)
local awarenessSizeIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT3
awarenessSizeIV = InspectorVariable.new("awarenessSize", awarenessSizeIVT, awarenessSize)
NewVariable(awarenessSizeIV)

awarenessSpeed = 0.4
awarenessVisualSpeed = 0.7
--local awarenessSpeedIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT
--awarenessSpeedIV = InspectorVariable.new("awarenessSpeed", awarenessSpeedIVT, awarenessSpeed)
--NewVariable(awarenessSpeedIV)

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

awareness_green = nil
awareness_yellow = nil
awareness_red = nil

awareness_green_name = "awareness_green_" .. gameObject:GetUID()
awareness_yellow_name = "awareness_yellow_" .. gameObject:GetUID()
awareness_red_name = "awareness_red_" .. gameObject:GetUID()

function Float3Length(v)
    return math.sqrt(v.x * v.x + v.y * v.y + v.z * v.z)
end

function Float3Difference(a, b)
    return float3.new(b.x - a.x, b.y - a.y, b.z - a.z)
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
        for i = 1, #patrolWaypoints do
            for j = 1, 3 do
                patrolOldWaypoints[i] = {}
                patrolOldWaypoints[i][j] = patrolWaypoints[i][j]
            end
        end
    else
        for i = 1, #patrolWaypoints do
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
        DispatchEvent(pathfinderUpdateKey, {patrolWaypoints, pingpong, componentTransform:GetPosition()})
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
    WORM = 5,
    DEAD = 6,
    VICTORY = 7
}

state = STATE.UNAWARE

function LookAtDirection(direction)
    position = componentTransform:GetPosition()
    target = float3.new(position.x + direction.x, position.y + direction.y, position.z + direction.z)

    componentTransform:LookAt(direction, float3.new(0, 1, 0))
end

awareness = 0
targetAwareness = 0
awarenessSource = nil
awarenessPosition = nil

isSeeingPlayer = false
seeingPosition = nil
seeingSource = nil

function CheckIfPointInCone(position)
    if Float3Distance(position, componentTransform:GetPosition()) > visionConeRadius then
        do
            return (false)
        end
    end

    diff = Float3Difference(componentTransform:GetPosition(), position)

    diff.y = 0

    angle = Float3Angle(diff, componentTransform:GetFront())

    angle = math.abs(math.deg(angle))
    
    Log(tostring(diff) .. " " .. tostring(componentTransform:GetFront()) .. " " .. tostring(angle) .. "\n")

    if angle < visionConeAngle / 2 then
        do
            return (true)
        end
    end

    do
        return (false)
    end
end

function ProcessVisualTrigger(position, gameObject)
    if not CheckIfPointInCone(position) then
        if isSeeingPlayer then
            seeingPosition = nil
            seeingSource = nil
            isSeeingPlayer = false
            SetTargetStateToUNAWARE()
        end
        do
            return
        end
    end

    isSeeingPlayer = true

    seeingPosition = position
    seeingSource = gameObject
    if state == STATE.UNAWARE then
        SetTargetStateToSUS()
    elseif state == STATE.SUS then
        SetTargetStateToAGGRO()
    end
end

function CheckAuditoryTriggerInRange(position, range)
    mypos = componentTransform:GetPosition()

    distance = Float3Distance(mypos, position)

    if distance < hearingRange + range then
        do
            return (true)
        end
    end

    do
        return (false)
    end
end

function ProcessSingleAuditoryTrigger(position, source)
    if state == STATE.UNAWARE then
        DispatchEvent("State_Suspicious", {position})
        do
            return
        end
    end

    if state == STATE.SUS then
        DispatchEvent("State_Aggressive", {source})
        do
            return
        end
    end
end

hadRepeatedAuditoryTriggerLastFrame = false
auditoryTriggerIsRepeating = false

function ProcessRepeatedAuditoryTrigger(position, source)
    hadRepeatedAuditoryTriggerLastFrame = true
    if state == state.UNAWARE then
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
        do
            return
        end
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
        do
            return
        end
    end

    targetAwareness = 0
end

function SetTargetStateToSUS()
    if state == STATE.AGGRO then
        do
            return
        end
    end

    targetAwareness = 1
end

function SetTargetStateToAGGRO()
    targetAwareness = 2
end

function SetStateToUNAWARE()
    if state == STATE.AGGRO then
        do
            return
        end
    end

    CheckAndRecalculatePath(true)
    local oldState = state
    state = STATE.UNAWARE
    awareness = 0
    targetAwareness = 0
    DispatchEvent("Change_State", {oldState, state}) -- fields[1] -> fromState; fields[2] -> toState;
end

function SetStateToSUS(position)
    if state == STATE.AGGRO then
        do
            return
        end
    end

    if static == true then
        DispatchEvent(pathfinderUpdateKey, {{}, false, componentTransform:GetPosition()})
        LookAtDirection(Float3Difference(componentTransform:GetPosition(), position))
    else
        DispatchEvent(pathfinderUpdateKey, {{position}, false, componentTransform:GetPosition()})
    end

    oldState = state
    state = STATE.SUS
    awareness = 1
    targetAwareness = 0
    DispatchEvent("Change_State", {oldState, state}) -- fields[1] -> fromState; fields[2] -> toState;
end

function SetStateToAGGRO(source)
    local oldState = state
    state = STATE.AGGRO
    awareness = 2
    targetAwareness = 2
    DispatchEvent("Change_State", {oldState, state}) -- fields[1] -> fromState; fields[2] -> toState;
end

function SetStateToWORM()
    local oldState = state
    state = STATE.WORM
    DispatchEvent("Change_State", {oldState, state}) -- fields[1] -> fromState; fields[2] -> toState;
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
    elseif key == "Player_Position" then
        ProcessVisualTrigger(fields[1], fields[2])
    elseif key == "Player_Attack" then
        if (fields[1] == gameObject) then
            Die()
        end
    elseif key == "Death_Mark" then
        if (fields[1] == gameObject) then
            deathMarkTime = fields[2]
            deathMarkTimer = 0.0
        end
    elseif key == "Knife_Hit" then
        if (fields[1] == gameObject) then
            Die()
        end
    elseif key == "Sadiq_Update_Target" then -- fields[1] -> target; targeted for (1 -> warning; 2 -> eat; 3 -> spit)
        if (fields[1] == gameObject) then
            if (fields[2] == 1) then
                StopMovement()
            elseif (fields[2] == 2) then
                Die()
            end
        end
    end
end

function StopMovement()
    if (componentRigidbody ~= nil) then
        componentRigidbody:SetLinearVelocity(float3.new(0, 0, 0))
    end
    SetStateToWORM()
end

function ConfigAwarenessBars()
    awareness_green = Find(awareness_green_name)
    awareness_yellow = Find(awareness_yellow_name)
    awareness_red = Find(awareness_red_name)
end

function UpdateAwarenessBars()
    position = componentTransform:GetPosition()
    awareness_green:GetTransform():SetPosition(float3.new(position.x + awarenessOffset.x,
        position.y + awarenessOffset.y, position.z + awarenessOffset.z))
    awareness_yellow:GetTransform():SetPosition(float3.new(position.x + awarenessOffset.x,
        position.y + awarenessOffset.y, position.z + awarenessOffset.z))
    awareness_red:GetTransform():SetPosition(float3.new(position.x + awarenessOffset.x, position.y + awarenessOffset.y,
        position.z + awarenessOffset.z))

    if awareness < 1 then
        awareness_green:GetTransform():SetScale(
            float3.new(awarenessSize.x, awarenessSize.y * awareness, awarenessSize.z))
        awareness_yellow:GetTransform():SetScale(float3.new(0, 0, 0))
        awareness_red:GetTransform():SetScale(float3.new(0, 0, 0))
    end

    if awareness >= 1 and awareness < 2 then
        awareness_green:GetTransform():SetScale(float3.new(0, 0, 0))
        awareness_yellow:GetTransform():SetScale(float3.new(awarenessSize.x, awarenessSize.y * (awareness - 1),
            awarenessSize.z))
        awareness_red:GetTransform():SetScale(float3.new(0, 0, 0))
    end

    if awareness == 2 then
        awareness_green:GetTransform():SetScale(float3.new(0, 0, 0))
        awareness_yellow:GetTransform():SetScale(float3.new(0, 0, 0))
        awareness_red:GetTransform():SetScale(float3.new(awarenessSize.x, awarenessSize.y, awarenessSize.z))
    end
end

function Start()
    CheckAndRecalculatePath(true)
    InstantiateNamedPrefab("awareness_green", awareness_green_name)
    InstantiateNamedPrefab("awareness_yellow", awareness_yellow_name)
    InstantiateNamedPrefab("awareness_red", awareness_red_name)

    componentRigidbody = gameObject:GetRigidBody()
end

oldSourcePos = nil

coneLight = gameObject:GetLight()

function Update(dt)
    if coneLight == nil then
        coneLight = gameObject:GetLight()
    end

    if coneLight ~= nil then
        coneLight:SetDirection(float3.new(-componentTransform:GetFront().x, -componentTransform:GetFront().y, -componentTransform:GetFront().z))
        coneLight:SetRange(visionConeRadius)
        coneLight:SetAngle(visionConeAngle / 2)
    end

    -- Death Mark (Weirding way)
    if (deathMarkTimer ~= nil) then
        deathMarkTimer = deathMarkTimer + dt
        if (deathMarkTimer >= deathMarkTime) then
            -- Audio here
            Die()
            return
        end
    end

    if awareness_green == nil then
        ConfigAwarenessBars()
    else
        UpdateAwarenessBars()
    end

    --Log(tostring(componentTransform:GetFront()) .. "\n")
    --Log(tostring(awareness) .. " " .. tostring(targetAwareness) .. " " .. tostring(isSeeingPlayer) .. "\n")

    if awareness < targetAwareness and isSeeingPlayer == true then
        awareness = awareness + awarenessVisualSpeed * dt
    elseif awareness < targetAwareness and isSeeingPlayer == false then
        awareness = awareness + awarenessSpeed * dt
    elseif awareness > targetAwareness then
        awareness = awareness - awarenessSpeed * dt
    end

    if awareness < 1.1 and awareness > 0.9 and state ~= STATE.SUS then
        if seeingSource ~= nil then
            DispatchEvent("State_Suspicious", {seeingPosition})
        else
            DispatchEvent("State_Suspicious", {awarenessPosition})
        end
    end

    if awareness < 0 then
        SetStateToUNAWARE()
    elseif awareness > 2 then
        if seeingSource ~= nil then
            DispatchEvent("State_Aggressive", {seeingSource})
        else
            DispatchEvent("State_Aggressive", {awarenessSource})
        end
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

    if state == STATE.AGGRO then
        s = nil
        if seeingSource ~= nil then
            s = seeingSource
        elseif awarenessSource ~= nil then
            s = awarenessSource
        end

        if s ~= nil and (oldSourcePos == nil or Float3Distance(oldSourcePos, s:GetTransform():GetPosition()) > 10) then
            if static == true then
                DispatchEvent(pathfinderUpdateKey, {{}, false, componentTransform:GetPosition()})
                LookAtDirection(Float3Difference(componentTransform:GetPosition(), s:GetTransform():GetPosition()))
            else
                DispatchEvent(pathfinderUpdateKey,
                    {{s:GetTransform():GetPosition()}, false, componentTransform:GetPosition()})
            end
            DispatchEvent("Target_Update", {s})
        end

        oldSourcePos = s:GetTransform():GetPosition()
    end

    _loop = loop
    if state == STATE.SUS or state == STATE.AGGRO then
        _loop = false
    end
    if (state ~= STATE.WORM) then
        if (state ~= STATE.AGGRO) then
            DispatchEvent(pathfinderFollowKey, {speed, dt, _loop})
        else
            DispatchEvent(pathfinderFollowKey, {chaseSpeed, dt, _loop})
        end
    end
end

------------------- Functions --------------------
function Die()

    DispatchEvent("Die", {gameObject})

    -- currentState = STATE.DEAD

    if (componentAnimator ~= nil) then
        componentAnimator:SetSelectedClip("Death")
    end

end

--------------------------------------------------

print("EnemyController.lua compiled successfully!")
