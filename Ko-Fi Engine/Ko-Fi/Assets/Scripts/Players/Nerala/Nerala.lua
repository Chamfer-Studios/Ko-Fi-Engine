------------------- Player events --------------------
Movement = {
	IDLE = 1,
	WALK = 2,
	RUN = 3,
	CROUCH = 4,
}
Action = {
	IDLE = 1,
	ATTACK = 2,
	AIM_PRIMARY = 3,
	AIM_SECONDARY = 4,
	AIM_ULTIMATE = 5,
}
------------------------------------------------------

------------------- Variables setter --------------------
target = nil
currentMovement = Movement.IDLE
currentAction = Action.IDLE

-- Globals --
characterID = 2
speed = 500.0

-- Primary ability --
dartCastRange = 50.0
dartCooldown = 10.0
drawDart = false

-- Secondary ability --

-- Ultimate ability --
ultimateCooldown = 10.0
------------------------------------------------------

------------------- Inspector setter --------------------
-- Globals --
local characterIDIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
characterIDIV = InspectorVariable.new("characterID", characterIDIVT, characterID)
NewVariable(characterIDIV)

local speedIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT
speedIV = InspectorVariable.new("speed", speedIVT, speed)
NewVariable(speedIV)

-- Primary ability --
local dartCastRangeIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT
dartCastRangeIV = InspectorVariable.new("dartCastRange", dartCastRangeIVT, dartCastRange)
NewVariable(dartCastRangeIV)

local dartCooldownIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT
dartCooldownIV = InspectorVariable.new("dartCooldown", dartCooldownIVT, dartCooldown)
NewVariable(dartCooldownIV)

local drawDartIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL
drawDartIV = InspectorVariable.new("drawDart", drawDartIVT, drawDart)
NewVariable(drawDartIV)

-- Secondary ability --

-- Ultimate ability --
local ultimateCooldownIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT
ultimateCooldownIV = InspectorVariable.new("ultimateCooldown", ultimateCooldownIVT, ultimateCooldown)
NewVariable(ultimateCooldownIV)
------------------------------------------------------

------------------- Animation setter --------------------
componentAnimator = gameObject:GetComponentAnimator()
if (componentAnimator ~= nil) then
	componentAnimator:SetSelectedClip("Idle")
end
animationDuration = 0.8
animationTimer = 0.0
isAttacking = false -- This should go, just here for animations
------------------------------------------------------

------------------- Audio setter --------------------
componentSwitch = gameObject:GetAudioSwitch()
------------------------------------------------------

------------------- Physics setter --------------------
componentRigidBody = gameObject:GetRigidBody()
rigidBodyFlag = true
------------------------------------------------------

------------------- Particles setter --------------------
mouseParticles = Find("Mouse Particles")
if (mouseParticles ~= nil) then
	mouseParticles:GetComponentParticle():StopParticleSpawn()
end
------------------------------------------------------

------------------- Movement logic --------------------
doubleClickDuration = 0.5
doubleClickTimer = 0.0
isDoubleClicking = false
------------------------------------------------------

-------------------- Methods ---------------------
-- Called each loop iteration
function Update(dt)
	-- Set Starting Position
	if (rigidBodyFlag == true) then 
		if (componentRigidBody ~= nil) then
			rigidBodyFlag = false
			componentRigidBody:SetRigidBodyPos(float3.new(componentTransform:GetPosition().x, 10, componentTransform:GetPosition().z))
		end
	end

	-- If in ultimate, dont update
	if (Find("Mosquito") ~= nil) then
		return
	end

	-- Animation timer
	if (isAttacking == true and componentAnimator ~= nil) then
		animationTimer = animationTimer + dt
		if (animationTimer >= animationDuration) then
			componentAnimator:SetSelectedClip("Idle")
			isAttacking = false
			animationTimer = 0.0
		end
	end

	-- Running state logic
	if (isDoubleClicking == true) then
		if (doubleClickTimer < doubleClickDuration) then
			doubleClickTimer = doubleClickTimer + dt
		else 
			isDoubleClicking = false
			doubleClickTimer = 0.0
		end
	end

	-- Click particles logic
	if (mouseParticles ~= nil) then
		mouseParticles:GetComponentParticle():StopParticleSpawn()
	end

	-- Primary ability cooldown
	if (dartTimer ~= nil) then
		dartTimer = dartTimer + dt
		if (dartTimer >= dartCooldown) then
			dartTimer = nil
		end
	end

	-- Secondary ability cooldown

	-- Ultimate ability cooldown
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
	DispatchGlobalEvent("Player_Position", { componentTransform:GetPosition(), gameObject })

	--Gather Inputs
	if (IsSelected() == true) then 
		-- Left Click
		if (GetInput(1) == KEY_STATE.KEY_DOWN) then
			-- Primary ability (Dart)
			if (currentAction == Action.AIM_PRIMARY and dartTimer == nil) then
				target = GetGameObjectHovered()
				if (Distance3D(target:GetTransform():GetPosition(), componentTransform:GetPosition()) <= dartCastRange) then
					if (target.tag == Tag.ENEMY) then
						FireDart()
					else
						print("You have to select an enemy first! (Dart - Nerala)")
					end
				else
					print("Out of range (Dart - Nerala)")
				end

			-- Secondary ability (Smoke bomb)
			elseif (currentAction == Action.AIM_SECONDARY) then

			-- Ultimate ability (hunter-seeker)
			elseif (ultimateTimer == nil and currentAction == Action.AIM_ULTIMATE) then
				Ultimate()
			end
		end
	
		-- Right Click
		if (GetInput(3) == KEY_STATE.KEY_DOWN) then
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
	
		-- H
		if (GetInput(5) == KEY_STATE.KEY_DOWN) then 
			currentAction = Action.IDLE
		end

		-- K
		if (GetInput(6) == KEY_STATE.KEY_DOWN) then 
			currentAction = Action.AIM_PRIMARY
		end	

		-- D
		if (GetInput(12) == KEY_STATE.KEY_DOWN) then 
			currentAction = Action.AIM_SECONDARY
		end	

		-- SPACE
		if (GetInput(4) == KEY_STATE.KEY_DOWN) then 
			currentAction = Action.AIM_ULTIMATE
		end

		-- C -> Toggle crouch
		if (GetInput(9) == KEY_STATE.KEY_DOWN) then 
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

	-- Draw primary ability range
	if (drawDart == true) then

	end

	-- Draw secondary ability range

	-- Draw ultimate ability range

end
--------------------------------------------------


----------------- Functions -----------------
function MoveToDestination(dt)
	local targetPos2D = { destination.x, destination.z }
	local pos2D = { componentTransform:GetPosition().x, componentTransform:GetPosition().z }
	local d = Distance2D(pos2D, targetPos2D)
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
		if (vec2[1] < 0) then
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

-- Primary ability
function FireDart()

	InstantiatePrefab("Dart") -- This should instance the prefab
	dartTimer = 0.0
	if (componentSwitch ~= nil) then
		componentSwitch:PlayTrack(0)
	end
	if (componentAnimator ~= nil) then
		componentAnimator:SetSelectedClip("Dart")
	end

	StopMovement()
end

-- Ultimate ability
function Ultimate()
	
	InstantiatePrefab("Mosquito") -- This should instance the prefab
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

----------------- Collisions -----------------
function OnTriggerEnter(go)

end
--------------------------------------------------

----------------- Math Functions -----------------
function Normalize(vec, distance)
	vec[1] = vec[1] / distance
	vec[2] = vec[2] / distance
	return vec
end

function Distance2D(a, b)
    local dx, dy = a[1] - b[1], a[2] - b[2]
	local sqrt = math.sqrt(dx * dx + dy * dy)
    return sqrt
end

function Distance3D(a, b)
    diff = { x = b.x - a.x, y = b.y - a.y, z = b.z - a.z }
	local sqrt = math.sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z)
    return sqrt
end
--------------------------------------------------

print("Nerala.lua compiled succesfully")