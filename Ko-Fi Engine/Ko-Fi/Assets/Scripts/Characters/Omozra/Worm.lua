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
sandShakeTime = 1

-------------------- Methods ---------------------

function Start()

    DispatchGlobalEvent("Worm_Spawn", {})

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

    healParticles = Find("Heal Particle")
    if (healParticles ~= nil) then
        healParticles:GetComponentParticle():StopParticleSpawn()
    end

    sandParticles = Find("Sand Particle")
    if (sandParticles ~= nil) then
        sandParticles:GetComponentParticle():StopParticleSpawn()
    end
end

-- Called each loop iteration
function Update(dt)
    if (startCalled == false) then
        Start()
        startCalled = true
    end

    if (healHit == nil) then
        healParticles:GetComponentParticle():StopParticleSpawn()
    end
    healHit = nil

    if (sandShakeTimer ~= nil) then
        sandShakeTimer = sandShakeTimer + dt
        if (currentState ~= State.SPIT and currentState ~= State.SPIT_HEAL) then
            if (math.abs(Distance3D(nextPosition, target:GetTransform():GetPosition())) >= 5) then
                nextPosition = target:GetTransform():GetPosition()
                if (sandParticles ~= nil) then
                    sandParticles:GetTransform():SetPosition(nextPosition)
                end
            end
        end
        if (sandShakeTimer >= sandShakeTime) then
            sandShakeTimer = nil
            if (componentAnimator ~= nil) then
                componentAnimator:SetSelectedClip(nextClip)
                componentTransform:SetPosition(nextPosition)
                if (nextRotation ~= nil) then
                    LookAtTarget(nextRotation)
                    nextRotation = nil
                end
            end
            if (sandParticles ~= nil) then
                sandParticles:GetComponentParticle():StopParticleSpawn()
            end
        end
    end

    -- Animation timer
    if (componentAnimator ~= nil) then
        if (componentAnimator:IsCurrentClipLooping() == false) then
            if (componentAnimator:IsCurrentClipPlaying() == false) then
                if (currentState == State.DEVOUR and sandShakeTimer == nil) then
                    DoDevour()
                elseif (currentState == State.EAT and sandShakeTimer == nil) then
                    DoUltimate()
                elseif (currentState == State.SPIT and sandShakeTimer == nil) then
                    DoSpit()
                elseif (currentState == State.SPIT_HEAL and sandShakeTimer == nil) then
                    DoPrimary()
                else
                    componentTransform:SetPosition(float3.new(0, -50, 0))
                end
            end
        end
    end
end

-- Primary spit in your face
function CastPrimary(thisTarget, omozraPos)

    target = thisTarget

    if (target == nil) then
        DispatchGlobalEvent("Omozra_Primary_Bugged", {})
        do
            return
        end
    end

    targetPos = target:GetTransform():GetPosition()
    -- Spawn with an offset to the direction of the heal
    local targetPos2D = {targetPos.x, targetPos.z}
    local pos2D = {omozraPos.x, omozraPos.z}
    local d = Distance(pos2D, targetPos2D)
    local vec2 = {targetPos2D[1] - pos2D[1], targetPos2D[2] - pos2D[2]}
    vec2 = Normalize(vec2, d)

    nextPosition = float3.new(targetPos.x + vec2[1] * 50, 0, targetPos.z + vec2[2] * 50)
    nextRotation = targetPos

    -- math.randomseed(os.time())
    -- randomOffsetX = 40
    -- randomOffsetZ = 40
    -- if (math.random(1, 2) == 1) then
    --    rngX = math.random(-randomOffsetX, -20)
    --    rngZ = math.random(-randomOffsetZ, -20)
    -- else
    --    rngX = math.random(20, randomOffsetX)
    --    rngZ = math.random(20, randomOffsetZ)
    -- end
    -- local a = float3.new(targetPos.x + rngX, targetPos.y, targetPos.z + rngZ)
    -- componentTransform:SetPosition(a)
    -- Log("X: " .. a.x .. " Y: " .. a.y .. " Z: " .. a.z .. "\n")

    if (componentAnimator ~= nil) then
        nextClip = "SpitHeal"
        sandShakeTimer = 0
    end
    if (sandParticles ~= nil) then
        sandParticles:GetTransform():SetPosition(nextPosition)
        sandParticles:GetComponentParticle():ResumeParticleSpawn()
    end

    ChangeTrack({0, 1})

    currentState = State.SPIT_HEAL
end

function DoPrimary()
    -- TODO: spit something in the direction of the player
    if (target == nil) then
        DispatchGlobalEvent("Omozra_Primary_Bugged", {})
        do
            return
        end
    end

    InstantiatePrefab("SpitHeal2")

    if (componentAnimator ~= nil) then
        componentAnimator:SetSelectedClip("SpitHealToIdle")
    end

    if (componentSwitch:IsAnyTrackPlaying() == false) then
        currentTrackID = -1
    end

    while (currentTrackID == 0 or currentTrackID == 1) do
        return
    end

    if (currentTrackID ~= 3) then
        ChangeTrack({3})
    end

    currentState = State.IDLE
end

-- Devour
function CastDevour(castedOn)

    target = castedOn

    if (target == nil) then
        DispatchGlobalEvent("Omozra_Secondary_Bugged", {})
        do
            return
        end
    end

    local targetPos = target:GetTransform():GetPosition()

    nextPosition = float3.new(targetPos.x, 0, targetPos.z)
    if (componentAnimator ~= nil) then
        nextClip = "Devour"
        sandShakeTimer = 0
    end
    if (sandParticles ~= nil) then
        sandParticles:GetTransform():SetPosition(nextPosition)
        sandParticles:GetComponentParticle():ResumeParticleSpawn()
    end

    ChangeTrack({0, 1})
    currentState = State.DEVOUR
end

function DoDevour()

    if (target == nil) then
        DispatchGlobalEvent("Omozra_Secondary_Bugged", {})
        do
            return
        end
    end

    if (target.tag == Tag.CORPSE) then
        DispatchGlobalEvent("Sadiq_Update_Target", {target, 4}) -- fields[1] -> target; targeted for (1 -> warning; 2 -> eat; 3 -> spit)
    else
        DispatchGlobalEvent("Sadiq_Update_Target", {target, 2}) -- fields[1] -> target; targeted for (1 -> warning; 2 -> eat; 3 -> spit)
    end
    componentAnimator:SetSelectedClip("DevourToIdle")

    -- TODO: Add particles, audio, etc.
    if (componentSwitch:IsAnyTrackPlaying() == false) then
        currentTrackID = -1
    end

    while (currentTrackID == 0 or currentTrackID == 1) do
        return
    end

    if (currentTrackID ~= 2) then
        ChangeTrack({2})
    end

    currentState = State.IDLE
end

-- Ultimate
function CastUltimate(castedOn)

    target = castedOn

    if (target == nil) then
        DispatchGlobalEvent("Omozra_Ultimate_Bugged", {})
        do
            return
        end
    end

    local targetPos = target:GetTransform():GetPosition()

    nextPosition = float3.new(targetPos.x, 0, targetPos.z)
    if (componentAnimator ~= nil) then
        nextClip = "Devour"
        sandShakeTimer = 0
    end
    if (sandParticles ~= nil) then
        sandParticles:GetTransform():SetPosition(nextPosition)
        sandParticles:GetComponentParticle():ResumeParticleSpawn()
    end

    ChangeTrack({0, 1})

    currentState = State.EAT
end

function DoUltimate()

    if (target == nil) then
        DispatchGlobalEvent("Omozra_Ultimate_Bugged", {})
        do
            return
        end
    end

    DispatchGlobalEvent("Sadiq_Update_Target", {target, 2}) -- fields[1] -> target; fields[2] -> step of devour ability (1 -> warning; 2 -> eat; 3 -> spit)

    if (componentAnimator ~= nil) then
        componentAnimator:SetSelectedClip("DevourToIdle")
    end

    -- TODO: Add particles, audio, etc.
    if (componentSwitch:IsAnyTrackPlaying() == false) then
        currentTrackID = -1
    end

    while (currentTrackID == 0 or currentTrackID == 1) do
        return
    end

    if (currentTrackID ~= 3) then
        ChangeTrack({3})
    end

    currentState = State.IDLE
end

-- Spit
function CastSpit(position)

    target = position

    if (target == nil) then
        DispatchGlobalEvent("Omozra_Ultimate_Recast_Bugged", {})
        do
            return
        end
    end

    -- This is just to stop the movement
    DispatchGlobalEvent("Sadiq_Update_Target", {target, 1}) -- fields[1] -> target; fields[2] -> step of devour ability (1 -> warning; 2 -> eat; 3 -> spit)

    nextPosition = position

    if (componentAnimator ~= nil) then
        nextClip = "Spit"
        sandShakeTimer = 0
    end
    if (sandParticles ~= nil) then
        sandParticles:GetTransform():SetPosition(nextPosition)
        sandParticles:GetComponentParticle():ResumeParticleSpawn()
    end

    ChangeTrack({0, 1})

    currentState = State.SPIT
end

function DoSpit()

    if (target == nil) then
        DispatchGlobalEvent("Omozra_Ultimate_Recast_Bugged", {})
        do
            return
        end
    end

    DispatchGlobalEvent("Sadiq_Update_Target", {target}) -- fields[1] -> target; fields[2] -> step of devour ability (1 -> warning; 2 -> eat; 3 -> spit) -- No need for fields[2] cause we will check for state.work

    if (componentAnimator ~= nil) then
        componentAnimator:SetSelectedClip("SpitToIdle")
    end

    -- TODO: Add particles, audio, etc.
    if (componentSwitch:IsAnyTrackPlaying() == false) then
        currentTrackID = -1
    end

    while (currentTrackID == 0 or currentTrackID == 1) do
        return
    end

    if (currentTrackID ~= 3) then
        ChangeTrack({3})
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
    elseif (key == "Sadiq_Heal") then -- fields[1] -> target; fields[2] -> pos;
        CastPrimary(fields[1], fields[2])
    elseif (key == "Spit_Heal_Hit") then
        ChangeTrack({4})
        if (healParticles ~= nil) then
            healParticles:GetTransform():SetPosition(float3.new(fields[1]:GetTransform():GetPosition().x,
                fields[1]:GetTransform():GetPosition().y + 13, fields[1]:GetTransform():GetPosition().z))
            healParticles:GetComponentParticle():ResumeParticleSpawn()
            healHit = true
        end
    end
end
--------------------------------------------------
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

function ChangeTrack(_trackList)
    size = 0
    for i in pairs(_trackList) do
        size = size + 1
    end

    index = math.random(size)

    if (componentSwitch ~= nil) then
        if (currentTrackID ~= -1) then
            componentSwitch:StopTrack(currentTrackID)
        end
        currentTrackID = _trackList[index]
        componentSwitch:PlayTrack(currentTrackID)
    end
end

print("Worm.lua compiled succesfully")
