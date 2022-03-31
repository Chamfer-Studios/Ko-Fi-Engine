------------------- Variables --------------------

characterID = 1
isWalking = false -- To play the steps track only once
target = nil

WalkState = {
   WALK = 1,
   CROUCH = 2,
   RUN = 3,
}

Action = {
	IDLE = 1,
	ATTACKING = 2,
	WALKING = 3,
	RUNNING = 4,
	AIMING = 5,
}

currentWalkState = WalkState.WALK
currentAction = Action.IDLE
speed = 500  -- consider Start()
maxKnives = 2
knifeCount = maxKnives

local speedIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT			-- IVT == Inspector Variable Type
local speedIV = InspectorVariable.new("speed", speedIVT, speed)
NewVariable(speedIV)

local maxKnivesIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
local maxKnivesIV = InspectorVariable.new("maxKnives", maxKnivesIVT, maxKnives)
NewVariable(maxKnivesIV)

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
			if (knifeCount > 0) then
				if (currentAction == Action.AIMING) then
					target = GetGameObjectHovered()
					if (target.tag == Tag.ENEMY) then
						Fire()
					end
				end
			else
				print("Out of ammo")
			end
		end

		if (GetInput(3) == KEY_STATE.KEY_DOWN) then -- Right Click
			goHit = GetGameObjectHovered()
			if (goHit ~= hit and goHit ~= gameObject) then
				destination = GetLastMouseClick()
				if (currentWalkState == WalkState.WALK and isDoubleClicking == true) then
					currentWalkState = WalkState.RUN
				else
					currentWalkState = WalkState.WALK
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
		if (GetInput(8) == KEY_STATE.KEY_DOWN) then -- X
			if (currentWalkState == WalkState.CROUCH) then
				currentWalkState = WalkState.WALK
				if (isWalking == true and componentSwitch ~= nil) then
					componentSwitch:StopTrack(2)
					componentSwitch:PlayAudio(1)
				end
			else
				currentWalkState = WalkState.CROUCH
				if (isWalking == true and componentSwitch ~= nil) then
					componentSwitch:StopTrack(1)
					componentSwitch:PlayAudio(2)
				end
			end
		end	
		if (GetInput(10) == KEY_STATE.KEY_DOWN) then -- R
			Reload()
		end
	end
end

function PostUpdate(dt)

end

-- Move to destination
function MoveToDestination(dt)

	local targetPos2D = { destination.x, destination.z }
	local pos2D = { componentTransform:GetPosition().x, componentTransform:GetPosition().z }
	local d = Distance(pos2D, targetPos2D)
	local vec2 = { targetPos2D[1] - pos2D[1], targetPos2D[2] - pos2D[2] }

	if (d > 0.5) then

		local s = speed
		if (currentWalkState == WalkState.CROUCH) then
			s = speed * 0.66
		elseif (currentWalkState == WalkState.RUN) then
			s = speed * 1.5
		end

		if (isWalking ~= true) then
			if (currentWalkState == WalkState.WALK) then
				if (componentSwitch ~= nil) then
					componentSwitch:PlayTrack(1)
				end
				if (componentAnimator ~= nil) then
					componentAnimator:SetSelectedClip("Walk")
				end
			elseif (currentWalkState == WalkState.CROUCH) then
				if (componentSwitch ~= nil) then
					componentSwitch:PlayTrack(2)
				end
				if (componentAnimator ~= nil) then
					componentAnimator:SetSelectedClip("Crouch")
				end
			elseif (currentWalkState == WalkState.RUNNING) then
				if (componentSwitch ~= nil) then
					componentSwitch:PlayTrack(1)
				end
				if (componentAnimator ~= nil) then
					componentAnimator:SetSelectedClip("Run")
				end
			end

			if (mouseParticles ~= nil) then
				mouseParticles:GetComponentParticle():ResumeParticleSpawn()
				mouseParticles:GetTransform():SetPosition(destination)
			end

			isWalking = true
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
		
		if (componentRigidBody ~= nil) then
			componentRigidBody:Set2DVelocity(float2.new(0,0))
		end
		destination = nil
		if (componentSwitch ~= nil) then
			componentSwitch:StopTrack(1)
		end
		if (componentAnimator ~= nil) then
			componentAnimator:PlayAnimation("Idle")
		end
		isWalking = false
	end
	-- Add ChangeAnimation() to check the speed of the rigid body
end

function Reload()
	knifeCount = maxKnives
	--TODO: Play audio
end

function IsSelected()
	
	id = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)

	if (id == characterID) then	
		return true
	else 
		return false
	end
end

function Fire()

	CreateGameObject("Knife")
	knifeCount = knifeCount - 1
	if (componentSwitch ~= nil) then
		componentSwitch:PlayTrack(0)
	end
end

-------------------- Setters ---------------------

function SetDestination(dest)

end

function SetTarget(tar)
	target = tar
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

print("Zhib.lua compiled succesfully")

-------- Scraps --------
--local components = gameObject:GetComponents()
--print(components[3].type) -- return it as an int, can't associate back to enum (I don't know how to anyway)

--GameState = require "Assets.Scripts.GameState"
--GameState:Update(1)
--print(GameState:GetGameState())