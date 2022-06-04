------------------- Variables --------------------
speed = 7000
destination = nil
lifeTime = 5
-------------------- Methods ---------------------

function Start()
    boxCollider = gameObject:GetBoxCollider() -- This is here instead of at "awake" so the order of component creation does not affect
    componentRigidBody = gameObject:GetRigidBody() -- This is here instead of at "awake" so the order of component creation does not affect
    componentSwitch = gameObject:GetAudioSwitch()
    trackList = {0, 1}
    ChangeTrack(trackList)
end

-- Called each loop iteration
function Update(dt)

    if (lifeTimer ~= nil) then
        if (lifeTimer >= lifeTime) then
            DeleteGameObject()
            do
                return
            end
        end
        lifeTimer = lifeTimer + dt
    end

    if (vec2 ~= nil) then
        MoveToDestination(dt)
    end
end

-- Collision Handler
function OnTriggerEnter(go)
    if (go.tag == Tag.PLAYER) then
        DispatchGlobalEvent("Enemy_Attack", {target, "Sardaukar"})
        DeleteGameObject()
    end
end

-- Move to destination
function MoveToDestination(dt)

    -- Movement
    if (componentRigidBody ~= nil) then
        componentRigidBody:SetLinearVelocity(float3.new(vec2[1] * speed * dt, 0, vec2[2] * speed * dt))
    end

    -- Rotation
    local rad = math.acos(vec2[2])
    if (vec2[1] < 0) then
        rad = rad * (-1)
    end
    componentTransform:SetRotation(float3.new(componentTransform:GetRotation().x, componentTransform:GetRotation().y,
        rad))
end

function EventHandler(key, fields)
    if (key == "Update_Enemy_Dart_Target") then
        if (fields[1] == gameObject:GetName()) then
            target = fields[2]
            if (target ~= nil) then
                enemyPos = fields[3]
                destination = target:GetTransform():GetPosition()
                local targetPos2D = {destination.x, destination.z}
                local enemyPos2D = {enemyPos.x, enemyPos.z}
                local d = Distance(enemyPos2D, targetPos2D)
                vec2 = {targetPos2D[1] - enemyPos2D[1], targetPos2D[2] - enemyPos2D[2]}
                vec2 = Normalize(vec2, d)
                if (componentRigidBody ~= nil) then
                    componentRigidBody:SetRigidBodyPos(float3.new(enemyPos.x + vec2[1] * 3, enemyPos.y + 20,
                        enemyPos.z + vec2[2] * 3))
                end
                lifeTimer = 0
            else
                DeleteGameObject()
            end
        end
    end
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

print("SardaukarDart.lua compiled succesfully\n")
Log("SardaukarDart.lua compiled succesfully\n")
