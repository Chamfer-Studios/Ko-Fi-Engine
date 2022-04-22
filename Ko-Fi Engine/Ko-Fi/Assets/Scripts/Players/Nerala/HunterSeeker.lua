------------------- Variables --------------------

characterID = 2
speed = 1000
lifeTime = 20.0	-- secs --iv required
lifeTimer = 0
destination = nil
target = nil
attackRange = 20.0 -- Maybe too big

componentRigidBody = gameObject:GetRigidBody()
componentAnimator = gameObject:GetComponentAnimator()
if (componentAnimator ~= nil) then
	componentAnimator:SetSelectedClip("Idle")
end

-------------------- Methods ---------------------

-- Called each loop iteration
function Update(dt)
	
	if (lifeTimer >= lifeTime) then
		DeleteGameObject()
		return
	else
		lifeTimer = lifeTimer + dt
	end

	if (target ~= nil) then
		if (Distance3D(target:GetTransform():GetPosition(), componentTransform:GetPosition()) <= attackRange) then
			Attack()
		else
			print("a")
			destination = target:GetTransform():GetPosition()
			MoveToDestination(dt)
		end
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
					if (mouseParticles ~= nil) then
						mouseParticles:GetComponentParticle():ResumeParticleSpawn()
						mouseParticles:GetTransform():SetPosition(destination)
					end
				else
					destination = GetLastMouseClick()
					if (mouseParticles ~= nil) then
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
		-- Movement
		local vec = float3.new(destination.x - pos.x, destination.y - pos.y, destination.z - pos.z)
		vec.x = vec.x / d
		vec.y = vec.y / d
		vec.z = vec.z / d
		if (componentRigidBody ~= nil) then
			componentRigidBody:Set2DVelocity(float2.new(vec.x * speed * dt, vec.z * speed * dt))
		end
		-- Rotation
		local rad = math.acos(vec.z)
		if (vec.x < 0)	then
			rad = rad * (-1)
		end
		componentTransform:SetRotation(float3.new(componentTransform:GetRotation().x, rad, componentTransform:GetRotation().z))
	else
		destination = nil
		if (componentRigidBody ~= nil) then
			componentRigidBody:Set2DVelocity(float2.new(0,0))
		end
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

function Attack()
	if (componentAnimator ~= nil) then
		componentAnimator:SetSelectedClip("Attack")
	end
	SetLuaVariableFromGameObject(target, "lethal", lethal)
end

function StopMovement()

	destination = nil
	if (componentRigidBody ~= nil) then
		componentRigidBody:Set2DVelocity(float2.new(0,0))
	end
	if (componentSwitch ~= nil) then
		componentSwitch:StopTrack(1)
	end
	--if (componentAnimator ~= nil) then
	--	componentAnimator:SetSelectedClip("Idle")
	--end
end

function Distance3D(a, b)

    diff = { x = b.x - a.x, y = b.y - a.y, z = b.z - a.z }
    return math.sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z)
end

--------------------------------------------------

print("HunterSeeker.lua compiled succesfully")