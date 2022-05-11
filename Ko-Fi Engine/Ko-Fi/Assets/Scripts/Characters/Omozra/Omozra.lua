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
State = {
	IDLE = 1,
	ATTACK = 2,
	AIM_PRIMARY = 3,
	AIM_SECONDARY = 4,
	AIM_ULTIMATE = 5,
	AIM_ULTIMATE_RECAST = 6,
	DEAD = 7,
}
---------------------------------------------------------

------------------- Variables setter --------------------
target = nil
currentMovement = Movement.IDLE
currentState = State.IDLE
maxHP = 3
currentHP = maxHP
iFrames = 1.5
iFramesTimer = nil

-- Globals --
characterID = 3
speed = 2000.0

-- Primary ability --


-- Secondary ability --
secondaryCastRange = 100.0
secondaryCooldown = 10.0
drawSecondary = false

-- Ultimate ability --
ultimateCastRange = 100.0
ultimateCooldown = 30.0
drawUltimate = false
ultimateRecastRange = 100.0
drawUltimateRecast = false

---------------------------------------------------------

-------------------- Movement logic ---------------------
doubleClickDuration = 0.5
doubleClickTimer = 0.0
isDoubleClicking = false
---------------------------------------------------------

------------------- Inspector setter --------------------
-- Globals --
--characterIDIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
--characterIDIV = InspectorVariable.new("characterID", characterIDIVT, characterID)
--NewVariable(characterIDIV)
--
--speedIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT
--speedIV = InspectorVariable.new("speed", speedIVT, speed)
--NewVariable(speedIV)
--
---- Primary ability --
--
--
--
---- Secondary ability --
--secondaryCastRangeIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT
--secondaryCastRangeIV = InspectorVariable.new("secondaryCastRange", secondaryCastRangeIVT, secondaryCastRange)
--NewVariable(secondaryCastRangeIV)
--
--drawSecondaryIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL
--drawSecondaryIV = InspectorVariable.new("drawSecondary", drawSecondaryIVT, drawSecondary)
--NewVariable(drawSecondaryIV)
--
---- Ultimate ability --
--ultimateCastRangeIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT
--ultimateCastRangeIV = InspectorVariable.new("ultimateCastRange", ultimateCastRangeIVT, ultimateCastRange)
--NewVariable(ultimateCastRangeIV)
--
--drawUltimateIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL
--drawUltimateIV = InspectorVariable.new("drawUltimate", drawUltimateIVT, drawUltimate)
--NewVariable(drawUltimateIV)
---------------------------------------------------------


----------------------- Methods -------------------------

function Start()

	componentAnimator = gameObject:GetComponentAnimator()
	if (componentAnimator ~= nil) then
		componentAnimator:SetSelectedClip("Idle")
	end

	mouseParticles = Find("Mouse Particles")
	if (mouseParticles ~= nil) then
		mouseParticles:GetComponentParticle():StopParticleSpawn()
	end

	componentRigidBody = gameObject:GetRigidBody()

	componentBoxCollider = gameObject:GetBoxCollider()

	componentSwitch = gameObject:GetAudioSwitch()
	currentTrackID = -1

	InstantiatePrefab("Worm")

	currentHP = maxHP
	DispatchGlobalEvent("Player_Health", { characterID, currentHP, maxHP })
end

-- Called each loop iteration
function Update(dt)

	if (lastRotation ~= nil) then
		componentTransform:LookAt(lastRotation, float3.new(0, 1, 0))
		if (currentState == State.AIM_PRIMARY) then
			componentTransform:SetRotation(float3:new(componentTransform:GetRotation().x, componentTransform:GetRotation().y - 90, componentTransform:GetRotation().z))
		end
	end

	if (ManageTimers(dt) == false) then
		return
	end

	-- Manage States
	if (destination ~= nil)	then
		MoveToDestination(dt)
		DispatchEvent("Pathfinder_FollowPath", { speed, dt, false })
		DispatchGlobalEvent("Player_Position", { componentTransform:GetPosition(), gameObject })
	end
	

	--Gather Inputs
	if (IsSelected() == true) then 

		-- Left Click
		if (GetInput(1) == KEY_STATE.KEY_DOWN) then

			-- Primary ability ()
			if (currentState == State.AIM_PRIMARY) then
				
			
			-- Secondary ability
			elseif (secondaryTimer == nil and currentState == State.AIM_SECONDARY) then
				target = GetGameObjectHovered()
				if (target.tag == Tag.ENEMY and Distance3D(target:GetTransform():GetPosition(), componentTransform:GetPosition()) <= secondaryCastRange) then
					if (componentAnimator ~= nil) then
						CastSecondary()
					end
				else
					print("Out of range")
				end

			-- Ultimate ability
			elseif (ultimateTimer == nil and currentState == State.AIM_ULTIMATE) then
				target = GetGameObjectHovered()
				if (target.tag == Tag.PLAYER and Distance3D(target:GetTransform():GetPosition(), componentTransform:GetPosition()) <= ultimateCastRange) then
					if (componentAnimator ~= nil) then
						CastUltimate() -- Ult step 2
					end
				else
					print("Out of range")
				end
			end
			elseif (currentState == State.AIM_ULTIMATE_RECAST) then
				GetGameObjectHovered() -- This is for the ability to go to the mouse Pos (it uses the target var)
				local mouse = GetLastMouseClick()
				if (Distance3D(mouse, componentTransform:GetPosition()) <= ultimateRecastRange) then
					target = mouse
					if (componentAnimator ~= nil) then
						RecastUltimate(mouse) -- Ult step 6
					end
				else
					print("Out of range")
				end
			end
		end
		
		if (currentState == State.AIM_ULTIMATE_RECAST) then -- Ult step 5
			return -- Chech if this breaks the compilation
		end

		-- Right Click
		if (GetInput(3) == KEY_STATE.KEY_DOWN) then
			goHit = GetGameObjectHovered()
			if (goHit ~= gameObject) then
				destination = GetLastMouseClick()
				DispatchEvent("Pathfinder_UpdatePath", { { destination }, true, componentTransform:GetPosition() })
				if (currentMovement == Movement.WALK and isDoubleClicking == true) then

					currentMovement = Movement.RUN

					if (componentSwitch ~= nil) then
						if (currentTrackID ~= -1) then
							componentSwitch:StopTrack(currentTrackID)
						end
						currentTrackID = 1
						componentSwitch:PlayTrack(currentTrackID)
					end
				else
					if (currentMovement == Movement.IDLE) then

						currentMovement = Movement.WALK

						if (componentSwitch ~= nil) then
							if (currentTrackID ~= -1) then
								componentSwitch:StopTrack(currentTrackID)
							end
							currentTrackID = 0
							componentSwitch:PlayTrack(currentTrackID)
						end
					end
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
			currentState = State.IDLE
			DispatchGlobalEvent("Player_Ability", { characterID, 0, 0 })
		end

		-- K
		if (GetInput(6) == KEY_STATE.KEY_DOWN) then 
			currentState = State.AIM_PRIMARY
			DispatchGlobalEvent("Player_Ability", { characterID, 1, 1 })
		end	

		-- D
		if (GetInput(12) == KEY_STATE.KEY_DOWN) then
			currentState = State.AIM_SECONDARY
			DispatchGlobalEvent("Player_Ability", { characterID, 2, 1 })
		end	

		-- SPACE
		if (GetInput(4) == KEY_STATE.KEY_DOWN) then -- Ult step 1
			currentState = State.AIM_ULTIMATE
			DispatchGlobalEvent("Player_Ability", { characterID, 3, 1 })
		end

		-- C -> Toggle crouch
		if (GetInput(9) == KEY_STATE.KEY_DOWN) then 
			if (currentMovement == Movement.CROUCH) then
				if (destination ~= nil) then
					currentMovement = Movement.WALK
					if (componentSwitch ~= nil) then
						if (currentTrackID ~= -1) then
							componentSwitch:StopTrack(currentTrackID)
						end
						currentTrackID = 0
						componentSwitch:PlayTrack(currentTrackID)
					end
				else
					currentMovement = Movement.IDLE
				end
			else
				if (currentMovement ~= Movement.IDLE and componentSwitch ~= nil) then
					if (currentTrackID ~= -1) then
						componentSwitch:StopTrack(currentTrackID)
						currentTrackID = -1
					end
				end
				currentMovement = Movement.CROUCH
			end
		end
	end

	-- Draw primary ability range

	-- Draw secondary ability range
	if (drawSecondary == true) then

	end

	-- Draw ultimate ability range
	if (drawUltimate == true) then

	end
end
--------------------------------------------------

------------------- Functions --------------------
function ManageTimers(dt)
	local ret = true

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
	if (secondaryTimer ~= nil) then
		secondaryTimer = secondaryTimer + dt
		if (secondaryTimer >= secondaryCooldown) then
			secondaryTimer = nil
			DispatchGlobalEvent("Player_Ability", { characterID, 2, 0 })
		end
	end

	-- Ultimate ability cooldown
	if (ultimateTimer ~= nil) then
		ultimateTimer = ultimateTimer + dt
		if (ultimateTimer >= ultimateCooldown) then
			ultimateTimer = nil
			DispatchGlobalEvent("Player_Ability", { characterID, 3, 0 })
		end
	end

	-- Animation timer
	if (componentAnimator ~= nil) then
		if (componentAnimator:IsCurrentClipLooping() == false) then
			if (componentAnimator:IsCurrentClipPlaying() == true) then
				ret = false
			else
				if (currentState == State.AIM_PRIMARY) then
					
				elseif (currentState == State.AIM_SECONDARY) then
					DoSecondary()
				elseif (currentState == State.AIM_ULTIMATE) then
					if (isWormDone == nil) then
						DoUltimate()
						isWormDone = false
					end
				elseif (currentState == State.AIM_ULTIMATE_RECAST) then
					if (isWormDone == nil) then
						DoUltimateRecast()
					end
				else 
					componentAnimator:SetSelectedClip("Idle")
				end
			end
		end
	end

	return ret
end

function MoveToDestination(dt)
	local targetPos2D = { destination.x, destination.z }
	local pos2D = { componentTransform:GetPosition().x, componentTransform:GetPosition().z }
	local d = Distance2D(pos2D, targetPos2D)
	local vec2 = { targetPos2D[1] - pos2D[1], targetPos2D[2] - pos2D[2] }
	
	if (d > 5.0) then
	
		if (componentAnimator ~= nil) then
			if (currentMovement == Movement.WALK) then
				componentAnimator:SetSelectedClip("Walk")
			elseif (currentMovement == Movement.CROUCH) then
				componentAnimator:SetSelectedClip("Crouch")
			elseif (currentMovement == Movement.RUN) then
				componentAnimator:SetSelectedClip("Run")
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
		if (d < 2) then
			s = s * 0.5
		end
	
		-- Movement
		vec2 = Normalize(vec2, d)
		if (componentRigidBody ~= nil) then
			componentRigidBody:SetLinearVelocity(float3.new(vec2[1] * s * dt, 0, vec2[2] * s * dt))
		end
	
		-- Rotation
		lastRotation = float3.new(vec2[1], 0, vec2[2])
		componentTransform:LookAt(lastRotation, float3.new(0, 1, 0))
	else
		if (componentAnimator ~= nil) then
			componentAnimator:SetSelectedClip("Idle")
		end
		StopMovement()
	end
end

function StopMovement()

	if (componentSwitch ~= nil and currentTrackID ~= -1) then
		componentSwitch:StopTrack(currentTrackID)
		currentTrackID = -1
	end

	currentMovement = Movement.IDLE
	
	destination = nil
	if (componentRigidBody ~= nil) then
		componentRigidBody:SetLinearVelocity(float3.new(0,0,0))
	end
end

function LookAtTarget(lookAt)
	local targetPos2D = { lookAt.x, lookAt.z }
	local pos2D = { componentTransform:GetPosition().x, componentTransform:GetPosition().z }
	local d = Distance2D(pos2D, targetPos2D)
	local vec2 = { targetPos2D[1] - pos2D[1], targetPos2D[2] - pos2D[2] }
	lastRotation = float3.new(vec2[1], 0, vec2[2])
	componentTransform:LookAt(lastRotation, float3.new(0, 1, 0))
end

function IsSelected()
	
	local id = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)

	if (id == characterID) then	
		return true
	end

	return false
end

-- Primary ability
function CastPrimary(position)

end

-- Secondary ability
function CastSecondary(position)

	componentAnimator:SetSelectedClip("Secondary")
	secondaryTimer = 0.0
	StopMovement()

	DispatchGlobalEvent("Player_Ability", { characterID, 2, 2 })
	LookAtTarget(position)
end

function DoSecondary() 

	if (componentSwitch ~= nil) then
		if (currentTrackID ~= -1) then
			componentSwitch:StopTrack(currentTrackID)
		end
		currentTrackID = 3
		componentSwitch:PlayTrack(currentTrackID)
	end	
	
	DispatchGlobalEvent("Worm_State", { 5, target })
	currentState = State.IDLE
end


-- Ultimate ability
function CastUltimate() -- Ult step 3

	componentAnimator:SetSelectedClip("UltimateStart")
	-- CD will start when recasting		
	StopMovement()
	
	DispatchGlobalEvent("Player_Ability", { characterID, 3, 2 })
	LookAtTarget(target:GetTransform():GetPosition())
end

function DoUltimate() -- Ult step 4
	-- This event is just for the player affected
	DispatchGlobalEvent("Omozra_Ultimate_Target", { target }) -- Target player has to stop all actions

	currentState = State.AIM_ULTIMATE_RECAST
end

function RecastUltimate(position)

	componentAnimator:SetSelectedClip("Worm_Recast")
	ultimateTimer = 0.0
	isWormDone = nil
	
	DispatchGlobalEvent("Player_Ability", { characterID, 3, 2 })
	LookAtTarget(position)
end

function DoUltimateRecast() -- Ult step 7
	-- This event is for the worm
	DispatchGlobalEvent("Omozra_Ultimate_Recast", { target }) -- Mouse position is the target this time
	currentState = State.IDLE
end

function TakeDamage(damage)
	if (damage == nil) then
		damage = 1
	end

	iFramesTimer = 0.0

	if (currentHP > 1) then
		currentHP = currentHP - damage
		DispatchGlobalEvent("Player_Health", { characterID, currentHP, maxHP })

		if (componentSwitch ~= nil) then
			if (currentTrackID ~= -1) then
				componentSwitch:StopTrack(currentTrackID)
			end
			currentTrackID = 4 -- Should be 5
			componentSwitch:PlayTrack(currentTrackID)
		end
	else
		Die()
	end
end

function Die()

	StopMovement()
	
	currentState = State.DEAD
	currentHP = 0
	DispatchGlobalEvent("Player_Health", { characterID, currentHP, maxHP })

	if (componentAnimator ~= nil) then
		componentAnimator:SetSelectedClip("Death")
	end
	if (componentSwitch ~= nil) then
		if (currentTrackID ~= -1) then
			componentSwitch:StopTrack(currentTrackID)
		end
		currentTrackID = 5 -- Should be 6
		componentSwitch:PlayTrack(currentTrackID)
	end
end
--------------------------------------------------

-------------------- Events ----------------------
function EventHandler(key, fields)
	
	if (key == "Omozra_Ultimate_Target" and fields[1] == gameObject) then -- fields[1] -> go;
		
        --currentState = State.WORM
    elseif (key == "Worm_Start_Animation_Done") then -- fields[1] -> nil;
		currentState = State.AIM_ULTIMATE_RECAST
    end
end
--------------------------------------------------

------------------ Collisions --------------------
function OnTriggerEnter(go)
	if (go.tag == Tag.ENEMY and iFramesTimer == nil) then
		TakeDamage(1)
	end
end

function OnCollisionEnter(go)
	
	if (go.tag == Tag.ENEMY and iFramesTimer == nil) then
		TakeDamage(1)
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

print("Omozra.lua compiled succesfully")

-------- Scraps --------
--local components = gameObject:GetComponents()
--print(components[3].type) -- return it as an int, can't associate back to enum (I don't know how to anyway)

--GameState = require "Assets.Scripts.GameState"
--GameState:Update(1)
--print(GameState:GetGameState())