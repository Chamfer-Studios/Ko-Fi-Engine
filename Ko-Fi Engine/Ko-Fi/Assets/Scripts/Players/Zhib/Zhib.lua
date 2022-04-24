----------------------------------------------------------------------------------------------------------------------------------
--																																--
--		*** Disclamer! ***																										--
--																																--
--		This script has several dependencies to work:																			--
--		1. The game object has to have a rigidBody																				--
--		2. There has to be a GameState.lua script somewhere in the scene in order to receive inputs								--
--		3. The game object has to have a Pathfinder.lua script in order to move													--
--		4. The animator, particles and audioSwitch components will be accessed if they exist but they are not a requirement		--
--																																--
----------------------------------------------------------------------------------------------------------------------------------

--------------------- Player events ---------------------

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
---------------------------------------------------------

------------------- Variables setter --------------------
target = nil
currentMovement = Movement.IDLE
currentAction = Action.IDLE

-- Globals --
characterID = 1
speed = 500.0

-- Primary ability --
knifeCastRange = 30.0
maxKnives = 2 -- Move to a Start() func!!!
knifeCount = maxKnives

-- Secondary ability --
decoyCastRange = 100.0
decoyCooldown = 10.0
drawDecoy = false

-- Ultimate ability --
ultimateRange = 50.0
ultimateCooldown = 30.0
drawUltimate = false
ultimateRangeExtension = ultimateRange * 0.5
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
local knifeCastRangeIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT
knifeCastRangeIV = InspectorVariable.new("knifeCastRange", knifeCastRangeIVT, knifeCastRange)
NewVariable(knifeCastRangeIV)

local maxKnivesIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
maxKnivesIV = InspectorVariable.new("maxKnives", maxKnivesIVT, maxKnives)
NewVariable(maxKnivesIV)

-- Secondary ability --
local decoyCastRangeIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT
decoyCastRangeIV = InspectorVariable.new("decoyCastRange", decoyCastRangeIVT, decoyCastRange)
NewVariable(decoyCastRangeIV)

local drawDecoyIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL
drawDecoyIV = InspectorVariable.new("drawDecoy", drawDecoyIVT, drawDecoy)
NewVariable(drawDecoyIV)

-- Ultimate ability --
local ultimateRangeIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT
ultimateRangeIV = InspectorVariable.new("ultimateRange", ultimateRangeIVT, ultimateRange)
NewVariable(ultimateRangeIV)

local drawUltimateIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL
drawUltimateIV = InspectorVariable.new("drawUltimate", drawUltimateIVT, drawUltimate)
NewVariable(drawUltimateIV)
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

	-- Secondary ability cooldown
	if (decoyTimer ~= nil) then
		decoyTimer = decoyTimer + dt
		if (decoyTimer >= decoyCooldown) then
			decoyTimer = nil
		end
	end

	-- Ultimate ability cooldown
	if (ultimateTimer ~= nil) then
		ultimateTimer = ultimateTimer + dt
		if (ultimateTimer >= ultimateCooldown) then
			ultimateTimer = nil
		end
	end
	
	-- Invisible logic
	if (invisibilityDuration ~= nil) then -- While invis he shouldn't be able to do anything
		invisibilityTimer = invisibilityTimer + dt
		if (invisibilityTimer >= invisibilityDuration) then
			-- Reappear
			invisibilityDuration = nil
			gameObject.active = true
		end
		return
	end
	
	-- Actions
	if (destination ~= nil)	then
		--MoveToDestination(dt)
		DispatchEvent("Pathfinder_FollowPath", { speed, dt, false })
		DispatchGlobalEvent("Player_Position", { componentTransform:GetPosition(), gameObject })
	end
	

	--Gather Inputs
	if (IsSelected() == true) then 
		-- Left Click
		if (GetInput(1) == KEY_STATE.KEY_DOWN) then

			-- Primary ability (Knife)
			if (currentAction == Action.AIM_PRIMARY) then
				if (knifeCount > 0) then
					target = GetGameObjectHovered()
					if (target.tag == Tag.ENEMY and Distance3D(target:GetTransform():GetPosition(), componentTransform:GetPosition()) <= knifeCastRange) then
						FireKnife()
					end
				end
			
			-- Secondary ability (Decoy)
			elseif (decoyTimer == nil and currentAction == Action.AIM_SECONDARY) then
				GetGameObjectHovered() -- This is for the decoy to go to the mouse Pos (it uses the target var)
				mousePos = GetLastMouseClick()
				if (Distance3D(mousePos, componentTransform:GetPosition()) <= decoyCastRange) then
					target = mousePos
					PlaceDecoy()
				else
					print("Out of range")
				end

			-- Ultimate ability (master yi)
			elseif (ultimateTimer == nil and currentAction == Action.AIM_ULTIMATE) then
				target = GetGameObjectHovered()
				if (target.tag == Tag.ENEMY) then
					if (Distance3D(target:GetTransform():GetPosition(), componentTransform:GetPosition()) <= ultimateRange) then
						mousePos = GetLastMouseClick()
						Ultimate(mousePos)
					end
				else
					print("Out of range")
				end
			end
		end
	
		-- Right Click
		if (GetInput(3) == KEY_STATE.KEY_DOWN) then
			goHit = GetGameObjectHovered()
			if (goHit ~= gameObject) then
				destination = GetLastMouseClick()
				DispatchEvent("Pathfinder_UpdatePath", { { destination }, true, componentTransform:GetPosition() })
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

		-- R -> For debugging purposes only (reload knives)
		if (GetInput(10) == KEY_STATE.KEY_DOWN) then 
			ReloadKnives()
		end
	end

	-- Draw primary ability range

	-- Draw secondary ability range
	if (drawDecoy == true) then

	end

	-- Draw ultimate ability range
	if (drawUltimate == true) then

	end
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

function ReloadKnives() -- For debugging purposes only
	knifeCount = maxKnives
end

function IsSelected()
	
	id = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)

	if (id == characterID) then	
		return true
	end

	return false
end

-- Primary ability
function FireKnife()

	InstantiatePrefab("Knife") -- This should instance the prefab
	knifeCount = knifeCount - 1
	if (componentSwitch ~= nil) then
		componentSwitch:PlayTrack(0)
	end
	if (componentAnimator ~= nil) then
		componentAnimator:SetSelectedClip("Knife")
	end

	StopMovement()
end

-- Secondary ability
function PlaceDecoy(mousePos) 
	InstantiatePrefab("Decoy")
	if (componentSwitch ~= nil) then
		--componentSwitch:PlayTrack(0)
	end
	if (componentAnimator ~= nil) then
		componentAnimator:SetSelectedClip("Decoy")
	end
	decoyTimer = 0.0
	StopMovement()
end

-- Ultimate ability
function Ultimate(mousePos)
	
	-- Get all enemies in range of the Mouse
	enemiesInRange = {}
	enemies = GetObjectsByTag(Tag.ENEMY)
	for i = 1, #enemies do
		if (Distance3D(enemies[i]:GetTransform():GetPosition(), mousePos) <= ultimateRange) then
			enemiesInRange[#enemiesInRange + 1] = enemies[i]
		end
	end

	-- If there are none, the ability isn't casted
	if (#enemiesInRange <= 0) then
		return
	end

	-- Check them all for adjacent enemies, different than the ones on the list and add them if there are anyway
	for i = 1, #enemiesInRange do

		for j = 1, #enemies do
			if (enemiesInRange[i] ~= enemies[j]) then
				if (Distance3D(enemiesInRange[i]:GetTransform():GetPosition(), enemies[j]:GetTransform():GetPosition()) <= ultimateRangeExtension) then
					
					isAlreadyInArray = false
					for k = 1, #enemiesInRange do
						if (enemiesInRange[k] == enemies[j]) then
							isAlreadyInArray = true
						end
					end

					if (not isAlreadyInArray) then 
						enemiesInRange[#enemiesInRange + 1] = enemies[j]
					end
				end
			end
		end		
	end

	deathMarkDuration = 0.3
	-- Set IN ORDER the death mark
	for i = 1, #enemiesInRange do
		SetLuaVariableFromGameObject(enemiesInRange[i], "deathMarkDuration", deathMarkDuration)
		deathMarkDuration = deathMarkDuration + 0.3
	end

	-- final pos = final target pos + Normalized(final target pos - initial pos) * d
	local targetPos2D = { enemiesInRange[#enemiesInRange]:GetTransform():GetPosition().x, enemiesInRange[#enemiesInRange]:GetTransform():GetPosition().z }
	local pos2D = { componentTransform:GetPosition().x, componentTransform:GetPosition().z }
	local d = Distance2D(pos2D, targetPos2D)
	local vec2 = { targetPos2D[1] - pos2D[1], targetPos2D[2] - pos2D[2] }	
	vec2 = Normalize(vec2, d)

	-- Add as reappear position the position from the last enemy who's gonna die
	local dist = 25
	reappearPosition = float3.new(enemiesInRange[#enemiesInRange]:GetTransform():GetPosition().x + vec2[1] * dist, componentTransform:GetPosition().y, enemiesInRange[#enemiesInRange]:GetTransform():GetPosition().z + vec2[2] * dist)

	-- Set timer equal to the longest dath mark timer to reappear
	gameObject.active = false
	invisibilityTimer = 0
	invisibilityDuration = deathMarkDuration

	gameObject:GetRigidBody():SetRigidBodyPos(reappearPosition)

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
		componentSwitch:StopTrack(1) -- Sprint & crouch should stop here too
	end
	--if (componentAnimator ~= nil) then
	--	componentAnimator:SetSelectedClip("Idle")
	--end
end
--------------------------------------------------

----------------- Collisions ---------------------
function OnTriggerEnter(go)
	if (go.tag == Tag.PROJECTILE) then
		knifeCount = knifeCount + 1
	end
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
    return math.sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z)
end
--------------------------------------------------

print("Zhib.lua compiled succesfully")

-------- Scraps --------
--local components = gameObject:GetComponents()
--print(components[3].type) -- return it as an int, can't associate back to enum (I don't know how to anyway)

--GameState = require "Assets.Scripts.GameState"
--GameState:Update(1)
--print(GameState:GetGameState())