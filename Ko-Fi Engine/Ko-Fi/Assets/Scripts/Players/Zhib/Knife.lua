------------------- Variables --------------------

speed = 300
destination = nil

-------------------- Methods ---------------------

function Start()
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
		componentRigidBody:SetRigidBodyPos(float3.new(playerPos.x + vec2[1] * 3, playerPos.y, playerPos.z + vec2[2] * 3))
	end
end

-- Called each loop iteration
function Update(dt)

	if (destination ~= nil) then
		MoveToDestination(dt)
		print(componentTransform:GetPosition())
	end
end

-- Collision Handler
function OnTriggerEnter(go)
	if (destination == nil and go.tag == Tag.PLAYER) then -- Using direct name instead of tags so other players can't pick it up
		DeleteGameObject()
	end
end

-- Move to destination
function MoveToDestination(dt)

	local targetPos2D = { destination.x, destination.z }
	local pos2D = { componentTransform:GetPosition().x, componentTransform:GetPosition().z }
	local d = Distance(pos2D, targetPos2D)
	local vec2 = { targetPos2D[1] - pos2D[1], targetPos2D[2] - pos2D[2] }
	print(pos2D[1], pos2D[2])

	if (d > 5.0) then

		-- Movement
		vec2 = Normalize(vec2, d)
		if (componentRigidBody ~= nil) then
			componentRigidBody:Set2DVelocity(float2.new(vec2[1] * speed * dt, vec2[2] * speed * dt))
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
			componentRigidBody:Set2DVelocity(float2.new(0,0))
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