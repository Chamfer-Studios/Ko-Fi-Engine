------------------- Variables --------------------
speed = 100
destination = nil
lifeTime = 10.0 -- secs --iv required
lifeTimer = 0
effectRadius = 50.0
effectFlag = true

-------------------- Methods ---------------------

function Start()
    destination = GetVariable("Nerala.lua", "target", INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT3) -- float 3
    if (destination ~= nil) then
        destination.y = 0.0
        player = GetVariable("Nerala.lua", "gameObject", INSPECTOR_VARIABLE_TYPE.INSPECTOR_GAMEOBJECT)
        componentSwitch = gameObject:GetAudioSwitch()
        currentTrackID = -1
        local playerPos = player:GetTransform():GetPosition()
        local targetPos2D = {destination.x, destination.z}
        local pos2D = {playerPos.x, playerPos.z}
        local d = Distance(pos2D, targetPos2D)
        local vec2 = {targetPos2D[1] - pos2D[1], targetPos2D[2] - pos2D[2]}
        vec2 = Normalize(vec2, d)
        componentTransform:SetPosition(
            float3.new(playerPos.x + vec2[1] * 5, playerPos.y + 10, playerPos.z + vec2[2] * 5))

        componentLight = gameObject:GetLight()
        if (componentLight ~= nil) then
            componentLight:SetRange(effectRadius)
        end

        smokeParticle = Find("Nerala Smoke Particle")
        if (smokeParticle ~= nil) then
            smokeParticle:GetComponentParticle():StopParticleSpawn()
            smokeParticle:GetComponentParticle():SetLoop(false)
        end
    else
        DispatchGlobalEvent("Nerala_Secondary_Bugged", {})
        DeleteGameObject()
    end
end

-- Called each loop iteration
function Update(dt)

    if (destination ~= nil) then
        MoveToDestination(dt)
        if (destination == nil) then
            OnArrive()
        end
    elseif (lifeTimer <= lifeTime) then

        lifeTimer = lifeTimer + dt

        if (effectFlag) then
            -- DispatchGlobalEvent("Auditory_Trigger", { componentTransform:GetPosition(), effectRadius, "single", gameObject })

            if (currentTrackID ~= 0 and currentTrackID ~= 1) then
                trackList = {0}
                ChangeTrack(trackList)
            end

            if (componentSwitch:IsAnyTrackPlaying() == false) then
                currentTrackID = -1
            end

            while (currentTrackID == 0) do
                return
            end

            if (currentTrackID ~= 1) then
                trackList = {1}
                ChangeTrack(trackList)
            end
        end
    else
        smokeParticle:GetComponentParticle():SetLoop(false)
        smokeParticle:GetComponentParticle():StopParticleSpawn()
        DispatchGlobalEvent("Smokebomb_End", {})
        DeleteGameObject()
    end
end

-- Move to destination
function MoveToDestination(dt)
    local pos = componentTransform:GetPosition()
    local d = Distance3D(destination, pos)
    if (d > 2.0) then
        -- Movement
        local vec = float3.new(destination.x - pos.x, destination.y - pos.y, destination.z - pos.z)
        vec.x = vec.x / d
        vec.y = vec.y / d
        vec.z = vec.z / d

        componentTransform:SetPosition(float3.new(pos.x + vec.x * speed * dt, pos.y + vec.y * speed * dt,
            pos.z + vec.z * speed * dt))
    else
        componentTransform:SetPosition(float3.new(pos.x, 0, pos.z))
        destination = nil

        -- if(componentSwitch:IsAnyTrackPlaying() == false) then          

        -- end
    end
end

function OnArrive()

    smokeParticle:GetComponentParticle():SetLoop(true)
    smokeParticle:GetComponentParticle():ResumeParticleSpawn()
    smokeParticle:GetTransform():SetPosition(float3.new(componentTransform:GetPosition().x,
        componentTransform:GetPosition().y + 1, componentTransform:GetPosition().z))
    if (componentLight ~= nil) then
        componentLight:SetAngle(360 / 2)
    end

    -- Audio of hitting the ground

    DispatchGlobalEvent("Smokebomb_Start", {componentTransform:GetPosition(), effectRadius})
end

-- Math
function Distance3D(a, b)

    diff = {
        x = b.x - a.x,
        y = b.y - a.y,
        z = b.z - a.z
    }
    return math.sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z)
end

function Normalize(vec, distance)

    vec[1] = vec[1] / distance
    vec[2] = vec[2] / distance

    return vec
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

print("Smokebomb.lua compiled succesfully")
