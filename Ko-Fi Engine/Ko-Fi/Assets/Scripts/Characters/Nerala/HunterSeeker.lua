------------------- Variables --------------------
characterID = 2
speed = 3000
lifeTime = 30.0 -- secs --iv required
lifeTimer = 0
destination = nil
target = nil
attackRange = 20.0 -- Maybe too big

-------------------- Methods ---------------------

function Start()

    componentRigidBody = gameObject:GetRigidBody() -- This is here instead of at "awake" so the order of component creation does not affect
    destination = GetVariable("Nerala.lua", "target", INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT3) -- float 3
    player = GetVariable("Nerala.lua", "gameObject", INSPECTOR_VARIABLE_TYPE.INSPECTOR_GAMEOBJECT) -- player = Find("Nerala")
    componentSwitch = gameObject:GetAudioSwitch()
    currentTrackID = -1
    if (componentSwitch ~= nil) then
        currentTrackID = 0
        componentSwitch:PlayTrack(currentTrackID)
    end
    local playerPos = player:GetTransform():GetPosition()
    local targetPos2D = {destination.x, destination.z}
    local pos2D = {playerPos.x, playerPos.z}
    local d = Distance(pos2D, targetPos2D)
    local vec2 = {targetPos2D[1], targetPos2D[2]}
    -- vec2 = Normalize(vec2, d)
    if (componentRigidBody ~= nil) then
        Log(vec2[1] .. vec2[2] .. "\n")
        componentRigidBody:SetRigidBodyPos(float3.new(vec2[1], 10, vec2[2]))
    end

    mouseParticles = Find("Mouse Particles")
    if (mouseParticles ~= nil) then
        mouseParticles:GetComponentParticle():StopParticleSpawn()
    end
end

-- Called each loop iteration
function Update(dt)

    if (componentSwitch ~= nil) then
        if (currentTrackID ~= 1) then
            componentSwitch:StopTrack(currentTrackID)
            currentTrackID = 1
            componentSwitch:PlayTrack(currentTrackID)
        end
    end

    if (lifeTimer >= lifeTime) then
        DeleteGameObject()
        DispatchGlobalEvent("Mosquito_Death", {})
        return
    end

    lifeTimer = lifeTimer + dt

    if (lastRotation ~= nil) then
        componentTransform:LookAt(lastRotation, float3.new(0, 1, 0))
    end

    if (target ~= nil) then
        destination = target:GetTransform():GetPosition()
        DispatchEvent("Pathfinder_UpdatePath", {{destination}, false, componentTransform:GetPosition()})
        MoveToDestination(dt)
    elseif (destination ~= nil) then
        MoveToDestination(dt)
    end

    -- Gather Inputs
    if (IsSelected() == true) then

        -- Right Click
        if (GetInput(3) == KEY_STATE.KEY_DOWN) then -- Right Click
            goHit = GetGameObjectHovered()
            if (goHit ~= gameObject) then
                if (goHit.tag == Tag.ENEMY) then
                    target = goHit
                    destination = goHit:GetTransform():GetPosition()
                    DispatchEvent("Pathfinder_UpdatePath", {{destination}, false, componentTransform:GetPosition()})
                    if (mouseParticles ~= nil) then
                        mouseParticles:GetComponentParticle():SetLoop(true)
                        mouseParticles:GetComponentParticle():ResumeParticleSpawn()
                        mouseParticles:GetTransform():SetPosition(destination)
                    end
                else
                    destination = GetLastMouseClick()
                    DispatchEvent("Pathfinder_UpdatePath", {{destination}, false, componentTransform:GetPosition()})
                    target = nil
                    if (mouseParticles ~= nil) then
                        mouseParticles:GetComponentParticle():SetLoop(true)
                        mouseParticles:GetComponentParticle():ResumeParticleSpawn()
                        mouseParticles:GetTransform():SetPosition(destination)
                    end
                end
            end
        end
    end
end

-- Move to destination
function MoveToDestination(dt)
    local pos = componentTransform:GetPosition()
    local d = Distance3D(destination, pos)

    if (d > 5.0) then

        -- Adapt speed on arrive
        if (d < 2.0) then
            speed = speed * 0.5
        end

        -- Movement
        local vec = float3.new(destination.x - pos.x, 0, destination.z - pos.z)
        vec.x = vec.x / d
        vec.y = vec.y / d
        vec.z = vec.z / d
        if (componentRigidBody ~= nil) then
            -- componentRigidBody:SetLinearVelocity(float3.new(vec.x * speed * dt, 0, vec.z * speed * dt))
            DispatchEvent("Pathfinder_FollowPath", {speed, dt, false})
        end

        -- Rotation
        lastRotation = float3.new(vec.x, 0, vec.z)
        componentTransform:LookAt(lastRotation, float3.new(0, 1, 0))

    else
        StopMovement()
        -- Audio tracks should stop here
    end
end

function IsSelected()

    id = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)

    if (id == characterID) then
        return true
    end

    return false
end

function StopMovement()
    destination = nil
    if (componentRigidBody ~= nil) then
        componentRigidBody:SetLinearVelocity(float3.new(0, 0, 0))
    end
end

function EventHandler(key, fields)
    if (key == "Stop_Movement") then
        StopMovement()
    end
end

function OnTriggerEnter(go)
    if (go.tag == Tag.ENEMY) then
        DispatchGlobalEvent("Mosquito_Hit", {go})
    end
end

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

print("HunterSeeker.lua compiled succesfully")
Log("HunterSeeker.lua compiled succesfully\n")
