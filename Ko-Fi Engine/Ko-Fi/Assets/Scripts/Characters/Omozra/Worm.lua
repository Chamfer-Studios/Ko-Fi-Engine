State = {
    IDLE = 1,
    EAT = 2,
    AWAIT = 3,
    SPIT = 4,
    DEVOUR = 5,
    SPIT_HEAL = 6
}

------------------- Variables --------------------

target = nil
currentState = State.IDLE
startCalled = false

-------------------- Methods ---------------------

function Start()
    omozra = GetVariable("Omozra.lua", "gameObject", INSPECTOR_VARIABLE_TYPE.INSPECTOR_GAMEOBJECT)
    componentTransform:SetPosition(float3.new(omozra:GetTransform():GetPosition().x, -50,
        omozra:GetTransform():GetPosition().z))

    if (particles ~= null) then
        particles = gameObject:GetComponentParticle()
        particles:StopParticleSpawn()
    end

    componentSwitch = gameObject:GetAudioSwitch()
    currentTrackID = -1

    componentAnimator = gameObject:GetParent():GetComponentAnimator()
    if (componentAnimator ~= nil) then
        componentAnimator:SetSelectedClip("Idle") -- Doesn't exists but I need it to be different
    end
end

-- Called each loop iteration
function Update(dt)
    if (startCalled == false) then
        Start()
        startCalled = true
    end

    -- if (lastRotation ~= nil) then
    --     componentTransform:LookAt(lastRotation, float3.new(0, 1, 0))
    -- end

    -- Animation timer
    if (componentAnimator ~= nil) then
        if (componentAnimator:IsCurrentClipLooping() == false) then
            if (componentAnimator:IsCurrentClipPlaying() == false) then
                if (currentState == State.DEVOUR) then
                    DoDevour()
                elseif (currentState == State.EAT) then
                    DoUltimate()
                elseif (currentState == State.SPIT) then
                    DoSpit()
                elseif (currentState == State.SPIT_HEAL) then
                    DoSpitHeal()
                else
                    componentTransform:SetPosition(float3.new(0, -50, 0))
                end
            end
        end
    end
end

-- Devour
function CastDevour(castedOn)

    target = castedOn

    local targetPos = target:GetTransform():GetPosition()
    componentTransform:SetPosition(float3.new(targetPos.x, 0, targetPos.z))

    if (componentAnimator ~= nil) then
        componentAnimator:SetSelectedClip("Devour")
    end

    ChangeTrack(0)

    currentState = State.DEVOUR
end

function DoDevour()

    DispatchGlobalEvent("Sadiq_Update_Target", {target, 2}) -- fields[1] -> target; targeted for (1 -> warning; 2 -> eat; 3 -> spit)

    componentAnimator:SetSelectedClip("DevourToIdle")

    -- TODO: Add particles, audio, etc.
    ChangeTrack(2)

    currentState = State.IDLE
end

-- Ultimate
function CastUltimate(castedOn)

    target = castedOn

    local targetPos = target:GetTransform():GetPosition()
    componentTransform:SetPosition(float3.new(targetPos.x, 0, targetPos.z))

    if (componentAnimator ~= nil) then
        componentAnimator:SetSelectedClip("Devour")
    end

    ChangeTrack(0)

    currentState = State.EAT
end

function DoUltimate()

    DispatchGlobalEvent("Sadiq_Update_Target", {target, 2}) -- fields[1] -> target; fields[2] -> step of devour ability (1 -> warning; 2 -> eat; 3 -> spit)

    if (componentAnimator ~= nil) then
        componentAnimator:SetSelectedClip("DevourToIdle")
    end

    -- TODO: Add particles, audio, etc.
    ChangeTrack(2)

    currentState = State.IDLE
end

-- Spit
function CastSpit(position)

    target = position
    componentTransform:SetPosition(position)

    -- This is just to stop the movement
    DispatchGlobalEvent("Sadiq_Update_Target", {target, 1}) -- fields[1] -> target; fields[2] -> step of devour ability (1 -> warning; 2 -> eat; 3 -> spit)

    if (componentAnimator ~= nil) then
        componentAnimator:SetSelectedClip("Spit")
    end

    ChangeTrack(0)

    currentState = State.SPIT
end

function DoSpit()

    DispatchGlobalEvent("Sadiq_Update_Target", {target}) -- fields[1] -> target; fields[2] -> step of devour ability (1 -> warning; 2 -> eat; 3 -> spit) -- No need for fields[2] cause we will check for state.work

    if (componentAnimator ~= nil) then
        componentAnimator:SetSelectedClip("SpitToIdle")
    end

    -- TODO: Add particles, audio, etc.
    ChangeTrack(3)

    currentState = State.IDLE
end

function CastSpitHeal(thisTarget, omozraPos)
    target = thisTarget
    targetPos = target:GetTransform():GetPosition()
    -- Spawn with an offset to the direction of the heal
    local targetPos2D = {target:GetTransform():GetPosition().x, target:GetTransform():GetPosition().z}
    local pos2D = {omozraPos.x, omozraPos.z}
    local d = Distance(pos2D, targetPos2D)
    local vec2 = {targetPos2D[1] - pos2D[1], targetPos2D[2] - pos2D[2]}
    vec2 = Normalize(vec2, d)

    math.randomseed(os.time())
    randomOffsetX = 40
    randomOffsetZ = 40
    if (math.random(1, 2) == 1) then
        rngX = math.random(-randomOffsetX, -20)
        rngZ = math.random(-randomOffsetZ, -20)
    else
        rngX = math.random(20, randomOffsetX)
        rngZ = math.random(20, randomOffsetZ)
    end
    local a = float3.new(targetPos.x + rngX, targetPos.y, targetPos.z + rngZ)
    componentTransform:SetPosition(a)
    -- Log("X: " .. a.x .. " Y: " .. a.y .. " Z: " .. a.z .. "\n")

    LookAtTarget(thisTarget:GetTransform():GetPosition())

    if (componentAnimator ~= nil) then
        componentAnimator:SetSelectedClip("SpitHeal")
    end

    currentState = State.SPIT_HEAL
    Log("CastSpitHeal done successfully\n")
end

function DoSpitHeal()
    -- TODO: spit something in the direction of the player
    InstantiatePrefab("SpitHeal")

    if (componentAnimator ~= nil) then
        componentAnimator:SetSelectedClip("SpitHealToIdle")
    end

    currentState = State.IDLE
end

-------------------- Events ----------------------
function EventHandler(key, fields)

    if (key == "Sadiq_Update_Target" and currentState ~= State.SPIT) then -- fields[1] -> target; targeted for (1 -> warning; 2 -> eat; 3 -> spit)
        if (fields[2] == 1) then
            if (fields[1].tag == Tag.ENEMY or fields[1].tag == Tag.CORPSE) then
                CastDevour(fields[1])
            elseif (fields[1].tag == Tag.PLAYER) then
                CastUltimate(fields[1])
            end
        elseif (fields[2] == 3) then
            CastSpit(fields[1])
        end
    elseif (key == "Sadiq_Heal") then
        CastSpitHeal(fields[1], fields[2])
    end
end
--------------------------------------------------

function ChangeTrack(index)
    if (componentSwitch ~= nil) then
        if (currentTrackID ~= -1) then
            componentSwitch:StopTrack(currentTrackID)
        end
        currentTrackID = index
        componentSwitch:PlayTrack(currentTrackID)
    end
end

function LookAtTarget(position)
    local targetPos2D = {position.x, position.z}
    local pos2D = {componentTransform:GetPosition().x, componentTransform:GetPosition().z}
    local d = Distance2D(pos2D, targetPos2D)
    local vec2 = {targetPos2D[1] - pos2D[1], targetPos2D[2] - pos2D[2]}
    lastRotation = float3.new(-vec2[1], 0, -vec2[2])
    componentTransform:LookAt(lastRotation, float3.new(0, 1, 0))
end

----------------- Math Functions -----------------
function Normalize(vec, distance)
    vec[1] = vec[1] / distance
    vec[2] = vec[2] / distance
    return vec
end

function Distance2D(a, b)
    local dx, dy = a[1] - b[1], a[2] - b[2]
    local sqrt = math.sqrt(dx * dx + dy * dy)
    return sqrt
end

function Distance3D(a, b)
    diff = {
        x = b.x - a.x,
        y = b.y - a.y,
        z = b.z - a.z
    }
    return math.sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z)
end

function Distance(a, b)

    local dx, dy = a[1] - b[1], a[2] - b[2]
    return math.sqrt(dx * dx + dy * dy)

end
--------------------------------------------------

print("Worm.lua compiled succesfully")
