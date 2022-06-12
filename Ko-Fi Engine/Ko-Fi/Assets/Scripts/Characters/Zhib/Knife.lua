------------------- Variables --------------------
destination = nil
isGrabbable = false
once = false
soundRange = 0

-------------------- Methods ---------------------
function Start()
    boxCollider = gameObject:GetBoxCollider() -- This is here instead of at "awake" so the order of component creation does not affect
    componentRigidBody = gameObject:GetRigidBody() -- This is here instead of at "awake" so the order of component creation does not affect
    componentSwitch = gameObject:GetAudioSwitch()
    currentTrackID = -1
    soundRange = GetVariable("Zhib.lua", "primarySoundRange", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
    target = GetVariable("Zhib.lua", "target", INSPECTOR_VARIABLE_TYPE.INSPECTOR_GAMEOBJECT)
    player = GetVariable("Zhib.lua", "gameObject", INSPECTOR_VARIABLE_TYPE.INSPECTOR_GAMEOBJECT)
    speed = GetVariable("Zhib.lua", "knifeSpeed", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
    playerPos = player:GetTransform():GetPosition()
    if (target ~= nil) then
        destination = target:GetTransform():GetPosition()
        local targetPos2D = {destination.x, destination.z}
        local pos2D = {playerPos.x, playerPos.z}
        local d = Distance(pos2D, targetPos2D)
        local vec2 = {targetPos2D[1] - pos2D[1], targetPos2D[2] - pos2D[2]}
        vec2 = Normalize(vec2, d)
        if (componentRigidBody ~= nil) then
            componentRigidBody:SetRigidBodyPos(float3.new(playerPos.x + vec2[1] * 3, playerPos.y + 10,
                playerPos.z + vec2[2] * 3))
        end
        destination = float3.new(destination.x + vec2[1] * 5, destination.y, destination.z + vec2[2] * 5)

        componentParticle = gameObject:GetComponentParticle()
        if (componentParticle ~= nil) then
            componentParticle:StopParticleSpawn()
        end
    else
        DispatchGlobalEvent("Zhib_Primary_Bugged", {})
        DeleteGameObject()
    end
end

-- Called each loop iteration
function Update(dt)

    if (lastRotation ~= nil) then
        componentTransform:LookAt(lastRotation, float3.new(0, 1, 0))
    end
    if (destination ~= nil) then
        MoveToDestination(dt)
    end
end

-- Collision Handler
function OnTriggerEnter(go)
    if (go.tag == Tag.ENEMY) then
        if (once == false) then
            once = true
            DispatchGlobalEvent("Knife_Hit", {go}) -- Events better than OnTriggerEnter() for the enemies (cause more than one different type of projectile can hit an enemy)
            DispatchGlobalEvent("Auditory_Trigger", {componentTransform:GetPosition(), soundRange, "single", player})

            if (componentSwitch ~= nil) then
                if (currentTrackID ~= -1) then
                    componentSwitch:StopTrack(currentTrackID)
                end

                currentTrackID = 0
                componentSwitch:PlayTrack(currentTrackID)
            end
        end
    elseif (go:GetName() == "Zhib" and isGrabbable == true) then -- Using direct name instead of tags so other players can't pick it up
        DispatchGlobalEvent("Knife_Grabbed", {})
        DeleteGameObject()
    end
end

-- Move to destination
function MoveToDestination(dt)

    local targetPos2D = {destination.x, destination.z}
    local pos2D = {componentTransform:GetPosition().x, componentTransform:GetPosition().z}
    local d = Distance(pos2D, targetPos2D)
    local vec2 = {targetPos2D[1] - pos2D[1], targetPos2D[2] - pos2D[2]}

    if (d > 2.0) then

        -- Adapt speed on arrive
        local s = speed
        if (d < 15.0) then
            s = s * 0.5
        end

        -- Movement
        vec2 = Normalize(vec2, d)

        if (target ~= nil) then
            local newDestination = target:GetTransform():GetPosition()
            if (Distance3D(destination, newDestination) >= 5) then
                destination = float3.new(newDestination.x + vec2[1] * 5, newDestination.y,
                    newDestination.z + vec2[2] * 5)
            end
        end

        if (componentRigidBody ~= nil) then
            componentRigidBody:SetLinearVelocity(float3.new(vec2[1] * s * dt, 0, vec2[2] * s * dt))
        end

        -- Rotation
        lastRotation = float3.new(vec2[1], 0, vec2[2])
        componentTransform:LookAt(lastRotation, float3.new(0, 1, 0))
    else
        StopMovement()
    end
end

function StopMovement()
    destination = nil
    isGrabbable = true -- Has arrived to the destination

    if (componentRigidBody ~= nil) then
        componentRigidBody:SetStatic()
        -- componentRigidBody:SetLinearVelocity(float3.new(0, 0, 0))
        componentTransform:SetPosition(float3.new(componentTransform:GetPosition().x, 3,
            componentTransform:GetPosition().z))
    end
    if (componentParticle ~= nil) then
        componentParticle:ResumeParticleSpawn()
    end

    gameObject.tag = Tag.PICKUP

    DispatchGlobalEvent("Knife_Grabbable", {})
end
----------------- Math Functions -----------------

function Normalize(vec, distance)

    vec[1] = vec[1] / distance
    vec[2] = vec[2] / distance

    return vec
end

function Distance(a, b)

    local dx, dy = a[1] - b[1], a[2] - b[2]
    return math.sqrt(dx * dx + dy * dy)

end

function Distance3D(a, b)
    diff = {
        x = b.x - a.x,
        y = b.y - a.y,
        z = b.z - a.z
    }
    return math.sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z)
end

print("Knife.lua compiled succesfully\n")
Log("Knife.lua compiled succesfully\n")
