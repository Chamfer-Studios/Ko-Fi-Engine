------------------- Variables --------------------

characterID = 2
target = nil

Movement = {
	IDLE = 1,
	WALK = 2,
	RUN = 3,
	CROUCH = 4,
}

Action = {
	IDLE = 1,
	ATTACK = 2,
	AIM_DART = 3,
	--
	AIM_ULTIMATE = 5,
}

currentMovement = Movement.IDLE
currentAction = Action.IDLE
speed = 500.0  -- consider Start()
dartCastRange = 50.0
dartCooldown = 10.0
ultimateCooldown = 10.0

local speedIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT			-- IVT == Inspector Variable Type
speedIV = InspectorVariable.new("speed", speedIVT, speed)
NewVariable(speedIV)

local dartCastRangeIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT
dartCastRangeIV = InspectorVariable.new("dartCastRange", dartCastRangeIVT, dartCastRange)
NewVariable(dartCastRangeIV)

local dartCooldownIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT
dartCooldownIV = InspectorVariable.new("dartCooldown", dartCooldownIVT, dartCooldown)
NewVariable(dartCooldownIV)

local ultimateCooldownIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT
ultimateCooldownIV = InspectorVariable.new("ultimateCooldown", ultimateCooldownIVT, ultimateCooldown)
NewVariable(ultimateCooldownIV)

local characterIDIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
characterIDIV = InspectorVariable.new("characterID", characterIDIVT, characterID)
NewVariable(characterIDIV)


componentAnimator = gameObject:GetComponentAnimator()
if (componentAnimator ~= nil) then
	componentAnimator:SetSelectedClip("Idle")
end
animationDuration = 0.8
animationTimer = 0.0
isAttacking = false -- This should go, just here for animations

componentSwitch = gameObject:GetAudioSwitch()
componentRigidBody = gameObject:GetRigidBody()

mouseParticles = Find("Mouse Particles")
if (mouseParticles ~= nil) then
	mouseParticles:GetComponentParticle():StopParticleSpawn()
end

doubleClickDuration = 0.5
doubleClickTimer = 0.0
isDoubleClicking = false

rigidBodyFlag = true

-------------------- Methods ---------------------

-- Called each loop iteration
function Update(dt)
	if (rigidBodyFlag == true) then -- Set Starting Position
		if (componentRigidBody ~= nil) then
			rigidBodyFlag = false
			componentRigidBody:SetRigidBodyPos(float3.new(componentTransform:GetPosition().x, 10, componentTransform:GetPosition().z))
		end
	end
	-- Timers & Helpers
	if (Find("Mosquito") ~= nil) then
		return
	end
	if (isAttacking == true and componentAnimator ~= nil) then
		animationTimer = animationTimer + dt
		if (animationTimer >= animationDuration) then
			componentAnimator:SetSelectedClip("Idle")
			isAttacking = false
			animationTimer = 0.0
		end
	end
	if (isDoubleClicking == true) then
		if (doubleClickTimer < doubleClickDuration) then
			doubleClickTimer = doubleClickTimer + dt
		else 
			isDoubleClicking = false
			doubleClickTimer = 0.0
		end
	end
	if (mouseParticles ~= nil) then
		mouseParticles:GetComponentParticle():StopParticleSpawn()
	end
	if (dartTimer ~= nil) then
		dartTimer = dartTimer + dt
		if (dartTimer >= dartCooldown) then
			dartTimer = nil
		end
	end
	if (ultimateTimer ~= nil) then
		ultimateTimer = ultimateTimer + dt
		if (ultimateTimer >= ultimateCooldown) then
			ultimateTimer = nil
		end
	end
	-- Actions
	if (destination ~= nil)	then
		MoveToDestination(dt)
	end
	
	--Gather Inputs
	if (IsSelected() == true) then 
		
		-- Left Click
		if (GetInput(1) == KEY_STATE.KEY_DOWN) then
	
			-- Knife
			if (currentAction == Action.AIM_PRIMARY and dartTimer == nil) then -- Fire Knife (infinite range for now)
				target = GetGameObjectHovered()
				if (target.tag == Tag.ENEMY) then
					FireDart()
				end
			-- SmokeBomb

			-- Ultimate
			elseif (ultimateTimer == nil and currentAction == Action.AIM_ULTIMATE) then
				Ultimate()
			end
		end
	
		-- Right Click
		if (GetInput(3) == KEY_STATE.KEY_DOWN) then -- Right Click
			goHit = GetGameObjectHovered()
			if (goHit ~= gameObject) then
				destination = GetLastMouseClick()
				if (currentMovement == Movement.WALK and isDoubleClicking == true) then
					currentMovement = Movement.RUN
				else
					currentMovement = Movement.WALK
					isDoubleClicking = true
				end
				if (mouseParticles ~= nil) then
					mouseParticles:GetComponentParticle():ResumeParticleSpawn()
					mouseParticles:GetTransform():SetPosition(destination)
				end
			end
		end
	
		if (GetInput(5) == KEY_STATE.KEY_DOWN) then -- H
			currentAction = Action.IDLE
		end
		if (GetInput(6) == KEY_STATE.KEY_DOWN) then -- K
			currentAction = Action.AIM_PRIMARY
		end	
		if (GetInput(12) == KEY_STATE.KEY_DOWN) then -- D
			currentAction = Action.AIM_SECONDARY
		end	
		if (GetInput(4) == KEY_STATE.KEY_DOWN) then -- SPACE
			currentAction = Action.AIM_ULTIMATE
		end
		if (GetInput(9) == KEY_STATE.KEY_DOWN) then -- C -> Toggle crouch
			if (currentMovement == Movement.CROUCH) then
				currentMovement = Movement.WALK
				if (componentSwitch ~= nil) then
					componentSwitch:StopTrack(2)
					componentSwitch:PlayAudio(1)
				end
			else
				currentMovement = Movement.CROUCH
				if (componentSwitch ~= nil) then
					componentSwitch:StopTrack(1)
					componentSwitch:PlayAudio(2)
				end
			end
		end
	end
end

-- Move to destination
function MoveToDestination(dt)

	local targetPos2D = { destination.x, destination.z }
	local pos2D = { componentTransform:GetPosition().x, componentTransform:GetPosition().z }
	local d = Distance(pos2D, targetPos2D)
	local vec2 = { targetPos2D[1] - pos2D[1], targetPos2D[2] - pos2D[2] }

	if (d > 5.0) then
	
		if (currentMovement ~= Movement.IDLE) then
			if (currentMovement == Movement.WALK) then
				if (componentSwitch ~= nil) then
					componentSwitch:PlayTrack(1)
				end
				if (componentAnimator ~= nil) then
					componentAnimator:SetSelectedClip("Walk")
				end
			elseif (currentMovement == Movement.CROUCH) then
				if (componentSwitch ~= nil) then
					componentSwitch:PlayTrack(2)
				end
				if (componentAnimator ~= nil) then
					componentAnimator:SetSelectedClip("Crouch")
				end
			elseif (currentMovement == Movement.RUN) then
				if (componentSwitch ~= nil) then
					componentSwitch:PlayTrack(1)
				end
				if (componentAnimator ~= nil) then
					componentAnimator:SetSelectedClip("Run")
				end
			end
		end
		
		-- Adapt speed
		local s = speed
		if (currentMovement == Movement.CROUCH) then
			s = speed * 0.66
		elseif (currentMovement == Movement.RUN) then
			s = speed * 1.5
		end
	
		-- Adapt speed on arrive
		if (d < 10) then
			s = s * 0.5
		end
	
		-- Movement
		vec2 = Normalize(vec2, d)
		if (componentRigidBody ~= nil) then
			componentRigidBody:Set2DVelocity(float2.new(vec2[1] * s * dt, vec2[2] * s * dt))
		end
	
		-- Rotation
		local rad = math.acos(vec2[2])
		if (vec2[1] < 0)	then
			rad = rad * (-1)
		end
		componentTransform:SetRotation(float3.new(componentTransform:GetRotation().x, rad, componentTransform:GetRotation().z))
	else
		
		StopMovement()
	end
	-- Add ChangeAnimation() to check the speed of the rigid body
end

function IsSelected()
	
	id = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)

	if (id == characterID) then	
		return true
	end

	return false
end

function FireDart()

	CreateGameObject("Dart") -- This should instance the prefab
	dartTimer = 0.0
	if (componentSwitch ~= nil) then
		componentSwitch:PlayTrack(0)
	end
	if (componentAnimator ~= nil) then
		componentAnimator:SetSelectedClip("Dart")
	end

	StopMovement()
end

function Ultimate()
	
	CreateGameObject("Mosquito") -- This should instance the prefab
	--if (componentSwitch ~= nil) then
	--	componentSwitch:PlayTrack(0)
	--end
	if (componentAnimator ~= nil) then
		componentAnimator:SetSelectedClip("Ultimate")
	end
	StopMovement()

	ultimateTimer = 0.0
end

function StopMovement()
	currentMovement = Movement.IDLE -- Stops aimings and all actions

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

function OnTriggerEnter(go)

	if (go.tag == Tag.PROJECTILE) then

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

function Distance3D(a, b)

    diff = { x = b.x - a.x, y = b.y - a.y, z = b.z - a.z }
    return math.sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z)
end

--------------------------------------------------

print("Nerala.lua compiled succesfully")