------------------- Variables --------------------

speed = 3000
lifeTime = 10.0	-- secs --iv required
lifeTimer = 0
effectRadius = 250.0
componentRigidBody = gameObject:GetRigidBody()
effectFlag = true

-------------------- Methods ---------------------

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
		componentRigidBody:SetStatic()
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
			componentRigidBody:Set2DVelocity(float2.new(vec.x * speed * dt, vec.z * speed * dt))
		end
	else
		destination = nil
		if (componentRigidBody ~= nil) then
			componentRigidBody:Set2DVelocity(float2.new(0,0))
		end
	end
end

function SetDestination()
	if (target ~= nil) then
		destination = GetLastMouseClick()
		local pos = componentTransform:GetPosition()
		local d = Distance3D(destination, pos)
		local vec = { destination.x - pos.x, destination.y - pos.y, destination.z - pos.z }
		vec = vec / d
		if (componentRigidBody ~= nil) then
			componentRigidBody:SetRigidBodyPos(float3.new(vec.x * 15, vec.y * 15, vec.z * 15))
		end
	end
end

function Distance3D(a, b)

    diff = { x = b.x - a.x, y = b.y - a.y, z = b.z - a.z }
    return math.sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z)
end

--------------------------------------------------

print("Decoy.lua compiled succesfully")