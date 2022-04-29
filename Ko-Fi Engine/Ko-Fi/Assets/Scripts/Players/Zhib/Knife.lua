------------------- Variables --------------------

speed = 300
destination = nil

-------------------- Methods ---------------------

function Start()
	boxCollider = gameObject:GetBoxCollider() -- This is here instead of at "awake" so the order of component creation does not affect
	componentRigidBody = gameObject:GetRigidBody() -- This is here instead of at "awake" so the order of component creation does not affect
	target = GetVariable("Zhib.lua", "target", INSPECTOR_VARIABLE_TYPE.INSPECTOR_GAMEOBJECT)
	player = GetVariable("Zhib.lua", "gameObject", INSPECTOR_VARIABLE_TYPE.INSPECTOR_GAMEOBJECT)
	local playerPos = player:GetTransform():GetPosition()
	destination = target:GetTransform():GetPosition()
	local targetPos2D = { destination.x, destination.z }
	local pos2D = { playerPos.x, playerPos.z }
	local d = Distance(pos2D, targetPos2D)
	local vec2 = { targetPos2D[1] - pos2D[1], targetPos2D[2] - pos2D[2] }
	vec2 = Normalize(vec2, d)
	if (componentRigidBody ~= nil) then		
		componentRigidBody:SetRigidBodyPos(float3.new(playerPos.x + vec2[1] * 3, playerPos.y + 10, playerPos.z + vec2[2] * 3))
	end
end

-- Called each loop iteration
function Update(dt)

	if (destination ~= nil) then
		MoveToDestination(dt)
	end
end

-- Collision Handler
function OnTriggerEnter(go)
	if (go.tag == Tag.ENEMY) then
		boxCollider:SetFilter("terrain")
		boxCollider:UpdateFilter()
	elseif (destination == nil and go.tag == Tag.PLAYER) then -- Using direct name instead of tags so other players can't pick it up
		DeleteGameObject()
	end
end

-- Move to destination
function MoveToDestination(dt)

	local targetPos2D = { destination.x, destination.z }
	local pos2D = { componentTransform:GetPosition().x, componentTransform:GetPosition().z }
	local d = Distance(pos2D, targetPos2D)
	local vec2 = { targetPos2D[1] - pos2D[1], targetPos2D[2] - pos2D[2] }

	if (d > 2.0) then

		-- Movement
		vec2 = Normalize(vec2, d)
		if (componentRigidBody ~= nil) then
			componentRigidBody:SetLinearVelocity(float3.new(vec2[1] * speed * dt, 0, vec2[2] * speed * dt))
		end

		-- Rotation
		local rad = math.acos(vec2[2])
		if(vec2[1] < 0)	then
			rad = rad * (-1)
		end
		componentTransform:SetRotation(float3.new(componentTransform:GetRotation().x, componentTransform:GetRotation().y, rad))
	else
		
		destination = nil
		if (componentRigidBody ~= nil) then
			componentRigidBody:SetLinearVelocity(float3.new(0,0,0))
			componentRigidBody:SetRigidBodyPos(float3.new(componentTransform:GetPosition().x, 0, componentTransform:GetPosition().z))
			
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

print("All good")