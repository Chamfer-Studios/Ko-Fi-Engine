------------------- Variables --------------------

characterID = 3
target = nil

Action = {
	IDLE = 1,
	ATTACKING = 2,
	WALKING = 3,
	RUNNING = 4,
	AIMING = 5,
}

currentAction = Action.IDLE
speed = 500  -- consider Start()

local speedIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT			-- IVT == Inspector Variable Type
local speedIV = InspectorVariable.new("speed", speedIVT, speed)
NewVariable(speedIV)

local characterIDIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
local characterIDIV = InspectorVariable.new("characterID", characterIDIVT, characterID)
NewVariable(characterIDIV)

componentAnimator = gameObject:GetComponentAnimator()
if (componentAnimator ~= nil) then
	componentAnimator:SetSelectedClip("Idle")
end
animationDuration = 0.8
animationTimer = 0.0
isAttacking = false -- This should go

componentSwitch = gameObject:GetAudioSwitch()
componentRigidBody = gameObject:GetRigidBody()

mouseParticles = Find("Mouse Particles")
if (mouseParticles ~= nil) then
	mouseParticles:GetComponentParticle():StopParticleSpawn()
end

doubleClickDuration = 0.5
doubleClickTimer = 0.0
isDoubleClicking = false

-------------------- Methods ---------------------

-- Called each loop iteration
function Update(dt)

	-- Timers
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

	if (destination ~= nil)	then
		MoveToDestination(dt)
	end

	if (IsSelected() == true) then --Gather Inputs

		if (GetInput(1) == KEY_STATE.KEY_DOWN) then -- Left Click
			-- Do abillity
		end

		if (GetInput(3) == KEY_STATE.KEY_DOWN) then -- Right Click
			goHit = GetGameObjectHovered()
			if (goHit ~= hit and goHit ~= gameObject) then
				destination = GetLastMouseClick()
				if (currentAction == Action.WALKING and isDoubleClicking == true) then
					currentAction = Action.RUNNING
				else
					currentAction = Action.WALKING
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
			currentAction = Action.AIMING
		end	
		if (GetInput(8) == KEY_STATE.KEY_DOWN) then -- X -> Toggle crouch
			if (currentAction == Action.CROUCHING) then
				currentAction = Action.WALKING
				if (componentSwitch ~= nil) then
					componentSwitch:StopTrack(2)
					componentSwitch:PlayAudio(1)
				end
			else
				currentAction = Action.CROUCHING
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

	if (d > 0.5) then

		if (currentAction ~= Action.IDLE) then
			if (currentAction == Action.WALKING) then
				if (componentSwitch ~= nil) then
					componentSwitch:PlayTrack(1)
				end
				if (componentAnimator ~= nil) then
					componentAnimator:SetSelectedClip("Walk")
				end
			elseif (currentAction == Action.CROUCHING) then
				if (componentSwitch ~= nil) then
					componentSwitch:PlayTrack(2)
				end
				if (componentAnimator ~= nil) then
					componentAnimator:SetSelectedClip("Crouch")
				end
			elseif (currentAction == Action.RUNNING) then
				if (componentSwitch ~= nil) then
					componentSwitch:PlayTrack(1)
				end
				if (componentAnimator ~= nil) then
					componentAnimator:SetSelectedClip("Run")
				end
			end
		end
		
		-- Adapt speed to walk state
		local s = speed
		if (currentAction == Action.CROUCHING) then
			s = speed * 0.66
		elseif (currentAction == Action.RUNNING) then
			s = speed * 1.5
		end

		-- Movement
		vec2 = Normalize(vec2, d)
		if (componentRigidBody ~= nil) then
			componentRigidBody:Set2DVelocity(float2.new(vec2[1] * s * dt, vec2[2] * s * dt))
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
		if (componentSwitch ~= nil) then
			componentSwitch:StopTrack(1)
		end
		if (componentAnimator ~= nil) then
			componentAnimator:PlayAnimation("Idle")
		end
	end
	-- Add ChangeAnimation() to check the speed of the rigid body
end

function IsSelected()
	
	id = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)

	if (id == characterID) then	
		return true
	else 
		return false
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

--------------------------------------------------

print("Omozra.lua compiled succesfully")