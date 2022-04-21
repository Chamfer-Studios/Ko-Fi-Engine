------------------- Variables --------------------

characterID = 1
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
	AIM_KNIFE = 3,
	AIM_DECOY = 4,
	AIM_ULTIMATE = 5,
}

currentMovement = Movement.IDLE
currentAction = Action.IDLE
speed = 500.0  -- consider Start()
maxKnives = 2
knifeCount = maxKnives
decoyCastRange = 50.0
decoyCooldown = 10.0
ultimateRange = 50.0
ultimateRangeExtension = ultimateRange * 0.5
ultimateCooldown = 30.0


local speedIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT			-- IVT == Inspector Variable Type
speedIV = InspectorVariable.new("speed", speedIVT, speed)
NewVariable(speedIV)

local maxKnivesIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
maxKnivesIV = InspectorVariable.new("maxKnives", maxKnivesIVT, maxKnives)
NewVariable(maxKnivesIV)

local decoyCastRangeIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT
decoyCastRangeIV = InspectorVariable.new("decoyCastRange", decoyCastRangeIVT, decoyCastRange)
NewVariable(decoyCastRangeIV)

local ultimateRangeIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT
ultimateRangeIV = InspectorVariable.new("ultimateRange", ultimateRangeIVT, ultimateRange)
NewVariable(ultimateRangeIV)

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
	if (decoyTimer ~= nil) then
		decoyTimer = decoyTimer + dt
		if (decoyTimer >= decoyCooldown) then
			decoyTimer = nil
		end
	end
	if (ultimateTimer ~= nil) then
		ultimateTimer = ultimateTimer + dt
		if (ultimateTimer >= ultimateCooldown) then
			ultimateTimer = nil
		end
	end
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
		MoveToDestination(dt)
	end
	
	--Gather Inputs
	if (IsSelected() == true) then 
		
		-- Left Click
		if (GetInput(1) == KEY_STATE.KEY_DOWN) then
	
			-- Knife
			if (currentAction == Action.AIM_KNIFE) then -- Fire Knife (infinite range for now)
				if (knifeCount > 0) then
					target = GetGameObjectHovered()
					if (target.tag == Tag.ENEMY) then
						FireKnife()
					end
				else
					print("Out of ammo")
				end
			
			-- Decoy
			elseif (decoyTimer == nil and currentAction == Action.AIM_DECOY) then
				target = GetGameObjectHovered() -- This is for the decoy to go to the mouse Pos (it uses the target var)
				local mousePos = GetLastMouseClick()
				if (Distance3D(mousePos, componentTransform:GetPosition()) <= decoyCastRange) then
					PlaceDecoy()
				end

			-- Ultimate
			elseif (ultimateTimer == nil and currentAction == Action.AIM_ULTIMATE) then
				target = GetGameObjectHovered()
				if (target.tag == Tag.ENEMY and Distance3D(target:GetTransform():GetPosition(), componentTransform:GetPosition()) <= ultimateRange) then
					mousePos = GetLastMouseClick()
					Ultimate(mousePos)
				end
			end
		end
	
		-- Right Click
		if (GetInput(3) == KEY_STATE.KEY_DOWN) then -- Right Click
			goHit = GetGameObjectHovered()
			if (goHit ~= hit and goHit ~= gameObject) then
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
			currentAction = Action.AIM_KNIFE
		end	
		if (GetInput(12) == KEY_STATE.KEY_DOWN) then -- D
			currentAction = Action.AIM_DECOY
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
		if (GetInput(10) == KEY_STATE.KEY_DOWN) then -- R -> For debugging purposes only
			Reload()
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

function Reload() -- For debugging purposes only
	knifeCount = maxKnives
end

function IsSelected()
	
	id = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)

	if (id == characterID) then	
		return true
	end

	return false
end

function FireKnife()

	CreateGameObject("Knife") -- This should instance the prefab
	knifeCount = knifeCount - 1
	if (componentSwitch ~= nil) then
		componentSwitch:PlayTrack(0)
	end
	if (componentAnimator ~= nil) then
		componentAnimator:SetSelectedClip("Knife")
	end

	StopMovement()
end

function PlaceDecoy(mousePos) 
	CreateGameObject("Decoy") -- This should instance the prefab
	if (componentSwitch ~= nil) then
		--componentSwitch:PlayTrack(0)
	end
	if (componentAnimator ~= nil) then
		componentAnimator:SetSelectedClip("Decoy")
	end
	decoyTimer = 0.0
	StopMovement()
end

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
	local d = Distance(pos2D, targetPos2D)
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
		componentSwitch:StopTrack(1)
	end
	--if (componentAnimator ~= nil) then
	--	componentAnimator:SetSelectedClip("Idle")
	--end
end

function OnTriggerEnter(go)

	if (go.tag == Tag.PROJECTILE) then
		knifeCount = knifeCount + 1
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

print("Zhib.lua compiled succesfully")

-------- Scraps --------
--local components = gameObject:GetComponents()
--print(components[3].type) -- return it as an int, can't associate back to enum (I don't know how to anyway)

--GameState = require "Assets.Scripts.GameState"
--GameState:Update(1)
--print(GameState:GetGameState())