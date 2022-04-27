------------------- Variables --------------------

speed = 1000
destination = nil
lifeTime = 10.0	-- secs --iv required
lifeTimer = 0
effectRadius = 250.0
effectFlag = true

-------------------- Methods ---------------------

function Start()
	componentRigidBody = gameObject:GetRigidBody() -- This is here instead of at "awake" so the order of component creation does not affect
	destination = GetVariable("Zhib.lua", "target", INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT3) -- float 3
	player = GetVariable("Zhib.lua", "gameObject", INSPECTOR_VARIABLE_TYPE.INSPECTOR_GAMEOBJECT)
	local playerPos = player:GetTransform():GetPosition()
	local targetPos2D = { destination.x, destination.z }
	local pos2D = { playerPos.x, playerPos.z }
	local d = Distance(pos2D, targetPos2D)
	local vec2 = { targetPos2D[1] - pos2D[1], targetPos2D[2] - pos2D[2] }
	vec2 = Normalize(vec2, d)
	if (componentRigidBody ~= nil) then		
		componentRigidBody:SetRigidBodyPos(float3.new(playerPos.x + vec2[1] * 3, 0, playerPos.z + vec2[2] * 3))
	end
end

-- Called each loop iteration
function Update(dt)
	
	if (destination ~= nil) then 
		MoveToDestination(dt)
	elseif (lifeTimer <= lifeTime) then

		lifeTimer = lifeTimer + dt

		if (effectFlag) then
			DispatchGlobalEvent("Auditory_Trigger", { componentTransform:GetPosition(), effectRadius, "single", gameObject })
			effectFlag = false
		end
	else
		DeleteGameObject()
	end
end

-- Move to destination
function MoveToDestination(dt)
	local pos = componentTransform:GetPosition()
	local d = Distance3D(destination, pos)
	if (d > 5.0) then
		-- Movement
		local vec = float3.new(destination.x - pos.x, destination.y - pos.y, destination.z - pos.z)
		vec.x = vec.x / d
		vec.y = vec.y / d
		vec.z = vec.z / d
		if (componentRigidBody ~= nil) then
			componentRigidBody:SetLinearVelocity(float3.new(vec.x * speed * dt, 0, vec.z * speed * dt))
		end
	else
		destination = nil
		if (componentRigidBody ~= nil) then
			componentRigidBody:SetLinearVelocity(float3.new(0,0,0))
		end
	end
end

function Distance3D(a, b)

    diff = { x = b.x - a.x, y = b.y - a.y, z = b.z - a.z }
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

print("Decoy.lua compiled succesfully")