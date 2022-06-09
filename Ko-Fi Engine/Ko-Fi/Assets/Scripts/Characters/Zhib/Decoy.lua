------------------- Variables --------------------
speed = 100
destination = nil
lifeTime = 10.0 -- secs --iv required
lifeTimer = 0
effectRadius = 100.0
effectTime = 0.0
isGrabbable = false
--------------------------------------------------

-------------------- Methods ---------------------
function Start()
    destination = GetVariable("Zhib.lua", "target", INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT3) -- float 3
    if (destination ~= nil) then
        destination.y = 0.0
        effectRadius = GetVariable("Zhib.lua", "secondaryEffectRadius", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        lifeTime = GetVariable("Zhib.lua", "secondaryDuration", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        player = GetVariable("Zhib.lua", "gameObject", INSPECTOR_VARIABLE_TYPE.INSPECTOR_GAMEOBJECT)
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
        waveParticleTimer = 1.0
        waveParticle = Find("Zhib Wave Particle")
        if (waveParticle ~= nil) then
            waveParticle:GetComponentParticle():StopParticleSpawn()
        end
        pickupParticles = gameObject:GetComponentParticle()
        if (pickupParticles ~= nil) then
            pickupParticles:StopParticleSpawn()
        end
        componentLight = gameObject:GetLight()
        if (componentLight ~= nil) then
            componentLight:SetRange(effectRadius)
        end
    else
        DispatchGlobalEvent("Zhib_Secondary_Bugged", {})
        DeleteGameObject()
    end
end

a = false

-- Called each loop iteration
function Update(dt)

    if (destination ~= nil) then
        MoveToDestination(dt)
    elseif (lifeTimer <= lifeTime) then

        if(currentTrackID ~= 0) then
            trackList = {0}
            ChangeTrack(trackList)
        end

        if (componentLight ~= nil) then
            -- componentLight:SetAngle(360 / 2)
        end

        lifeTimer = lifeTimer + dt

        if (effectTimer == nil) then
            waveParticleTimer = waveParticleTimer + dt
            if (waveParticleTimer > 1.0) then
                waveParticleTimer = 0.0
                if (waveParticle ~= nil) then
                    waveParticle:GetComponentParticle():ResumeParticleSpawn()
                    waveParticle:GetTransform():SetPosition(float3.new(componentTransform:GetPosition().x,
                        componentTransform:GetPosition().y + 1, componentTransform:GetPosition().z))
                end
            end
            
            DispatchGlobalEvent("Auditory_Trigger", {componentTransform:GetPosition(), effectRadius, "decoy", player})
            effectTimer = 0.0
        else
            effectTimer = effectTimer + dt
            if (effectTimer >= effectTime) then
                effectTimer = nil
            end
        end
    else
        if (componentLight ~= nil) then
            componentLight:SetAngle(0)
        end
        if (currentTrackID ~= -1) then
            componentSwitch:StopTrack(currentTrackID)
        end
        if (waveParticle ~= nil) then
            waveParticle:GetComponentParticle():StopParticleSpawn()
        end
        if (pickupParticles ~= nil) then
            pickupParticles:ResumeParticleSpawn()
        end
        -- Log("Decoy is grabbable! \n")

        if a == false then
            DispatchGlobalEvent("Decoy_Trigger_End", {})
            DispatchGlobalEvent("Decoy_Grabbable", {})
            gameObject.tag = Tag.PICKUP
            a = true
        end

        isGrabbable = true
    end
end

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
    end
end

function OnTriggerEnter(go)

    if (go.tag == Tag.PLAYER and isGrabbable == true and go:GetName() == "Zhib") then
        DispatchGlobalEvent("Decoy_Grabbed", {})
        DeleteGameObject()
    end
end
--------------------------------------------------

----------------- Math Functions -----------------
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

print("Decoy.lua compiled succesfully")
Log("Decoy.lua compiled succesfully")
