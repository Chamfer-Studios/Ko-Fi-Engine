------------------- Variables --------------------

lifeTime = 6.0
speed = 200

-------------------- Methods ---------------------

function Start()
	lifeTimer = 0.0
	boxCollider = gameObject:GetBoxCollider() -- This is here instead of at "awake" so the order of component creation does not affect
	componentRigidBody = gameObject:GetRigidBody() -- This is here instead of at "awake" so the order of component creation does not affect
	initialPos = float3.new(0.0,1.5,0.0)
	initialScale = float3.new(3.0,1.0,1.0)
	destination = true
	if (componentRigidBody ~= nil) then		
		componentRigidBody:SetRigidBodyPos(initialPos)
		componentTransform:SetScale(initialScale)	
	end
end

-- Called each loop iteration
function Update(dt)

	-- SPACE (reset)
	if (GetInput(4) == KEY_STATE.KEY_DOWN and componentRigidBody ~= nil) then 
		Reset()
	end
	if (destination ~= nil) then
		MoveToDestination(dt)
	end
	if (lifeTimer >= lifeTime) then
		DeleteGameObject()
	end
end

-- Move to destination
function MoveToDestination(dt)

	-- Movement
	if (componentRigidBody ~= nil) then
		local mov = float3.new(0.0,0.0,-1.0)
		mov.z = mov.z * speed * dt
		componentRigidBody:SetLinearVelocity(mov)
	end

	-- Scale
	local scale = componentTransform:GetScale()
	componentTransform:SetScale(float3.new(scale.x + 2.0 * dt, scale.y, scale.z))
end

function Reset()
	componentRigidBody:SetRigidBodyPos(initialPos)
	componentTransform:SetScale(initialScale)
end

print("Projectile.lua")