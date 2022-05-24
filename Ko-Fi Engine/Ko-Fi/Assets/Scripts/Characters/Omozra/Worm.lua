State = {
    IDLE = 1,
    EAT = 2,
    AWAIT = 3,
    SPIT = 4,
    DEVOUR = 5
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
                else
                    componentTransform:SetPosition(float3.new(0, -50, 0))
                end
            end
        end
    end
end

-- Devour/Eat
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

-------------------- Events ----------------------
function EventHandler(key, fields)

    if (key == "Sadiq_Update_Target" and currentState ~= State.SPIT) then -- fields[1] -> target; targeted for (1 -> warning; 2 -> eat; 3 -> spit)
        if (fields[2] == 1) then
            if (fields[1].tag == Tag.ENEMY) then
                CastDevour(fields[1])
            elseif (fields[1].tag == Tag.PLAYER) then
                CastUltimate(fields[1])
            end
        elseif (fields[2] == 3) then
            CastSpit(fields[1])
        end
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

print("Worm.lua compiled succesfully")
