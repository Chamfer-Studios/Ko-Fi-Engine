------------------- Variables --------------------

characterID = 1
isWalking = false -- To play the steps track only once

State = {
   IDLE = 1,
   CROUCH = 2,
   PRONE = 3,
}

currentState = State.IDLE
speed = 500  -- consider Start()
maxBullets = 2
bulletCount = maxBullets

local speedIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT			-- IVT == Inspector Variable Type
local speedIV = InspectorVariable.new("speed", speedIVT, speed)
NewVariable(speedIV)

local maxBulletsIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
local maxBulletsIV = InspectorVariable.new("maxBullets", maxBulletsIVT, maxBullets)
NewVariable(maxBulletsIV)

local currentItemType = ItemType.ITEM_HAND
local currentItemDamage = 5
currentItem = Item.new(currentItemType, currentItemDamage)

componentAnimator = gameObject:GetComponentAnimator()
if (componentAnimator ~= nil) then
	componentAnimator:SetSelectedClip("Idle")
end
--animationDuration = 0.8
--animationTimer = 0.0
isAttacking = false
isAiming = false -- This is for GameState.lua not to unselect the player when you Left Click

componentSwitch = gameObject:GetAudioSwitch()
componentRigidBody = gameObject:GetRigidBody()

mouseParticles = Find("Mouse Particles")
if (mouseParticles ~= nil) then
	mouseParticles:GetComponentParticle():StopParticleSpawn()
end

particleFlag = false

target = nil

-------------------- Methods ---------------------

-- Called each loop iteration
function Update(dt)

	if (isAttacking and componentAnimator ~= nil) then
		animationTimer = animationTimer + dt
		if (animationTimer >= animationDuration) then
			componentAnimator:SetSelectedClip("Idle")
			isAttacking = false
			animationTimer = 0.0
		end
	end
	if (particleFlag == true and mouseParticles ~= nil) then
		mouseParticles:GetComponentParticle():StopParticleSpawn()
		particleFlag = false
	end

	if (destination ~= nil)	then
		MoveToDestination(dt)
	end

	if (IsSelected() == true)
		then --Gather Inputs
			if (GetInput(1) == KEY_STATE.KEY_DOWN) then -- Left Click
				
				if (currentItem.type == ItemType.ITEM_GUN and bulletCount > 0) then

					target = GetGameObjectHovered()
					if (target.tag == Tag.ENEMY) then
						Fire()
					end
				end
			end
			if (GetInput(3) == KEY_STATE.KEY_DOWN) then -- RightClick
				if (mouseParticles ~= nil) then
					mouseParticles:GetComponentParticle():ResumeParticleSpawn()
					mouseParticles:GetTransform():SetPosition(destination)
					particleFlag = true
				end
			end
			if (GetInput(5) == KEY_STATE.KEY_DOWN) then -- H
				currentItem.type = ItemType.ITEM_HAND
				isAiming = false
			end
			if (GetInput(6) == KEY_STATE.KEY_DOWN) then -- K
				currentItem.type = ItemType.ITEM_KNIFE
				isAiming = false
			end
			if (GetInput(7) == KEY_STATE.KEY_DOWN) then -- G
				currentItem.type = ItemType.ITEM_GUN
				isAiming = true
			end	
			if (GetInput(8) == KEY_STATE.KEY_DOWN) -- X
				then
					if (currentState == State.CROUCH) then
						currentState = State.IDLE
						if (isWalking == true and componentSwitch ~= nil) then
							componentSwitch:StopTrack(2)
							componentSwitch:PlayAudio(1)
						end
					else
						currentState = State.CROUCH
						if (isWalking == true and componentSwitch ~= nil) then
							componentSwitch:StopTrack(1)
							componentSwitch:PlayAudio(2)
						end
					end
			end	
			--if (GetInput(9) == KEY_STATE.KEY_DOWN)  -- C
			--	then
			--		if (currentState == State.PRONE) then
			--			currentState = State.IDLE
			--			-- TODO: Play audio
			--		else
			--			currentState = State.PRONE
			--			-- TODO: Play audio
			--		end	
			--end
			if (GetInput(10) == KEY_STATE.KEY_DOWN) then -- R
				Reload()
			end
			if (GetInput(4) == KEY_STATE.KEY_DOWN) -- SPACE
				then
					if (currentItem.type == ItemType.ITEM_GUN and bulletCount > 0) then
						-- Cast a ray to check if it can shoot a target
						CreateBullet()
						bulletCount = bulletCount - 1
						if (componentSwitch ~= nil) then
							componentSwitch:PlayTrack(0)
						end
					elseif (currentItem.type == ItemType.ITEM_KNIFE) then
						print("La cartera, bro")
					elseif (currentItem.type == ItemType.ITEM_NO_TYPE) then
						print("No item selected")
				end
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

	if (d > 0.5)
		then

			local s = speed
			if (currentState == State.CROUCH) then
				s = speed * 0.66
			elseif (currentState == State.PRONE) then
				s = speed * 0.33
			end

			if (isWalking ~= true) then
				if (currentState == State.IDLE) then
					if (componentSwitch ~= nil) then
						componentSwitch:PlayTrack(1)
					end
					if (componentAnimator ~= nil) then
						componentAnimator:SetSelectedClip("Walk")
					end
				elseif (currentState == State.CROUCH) then
					if (componentSwitch ~= nil) then
						componentSwitch:PlayTrack(2)
					end
					if (componentAnimator ~= nil) then
						componentAnimator:SetSelectedClip("Crouch")
					end
				end

				if (mouseParticles ~= nil) then
					mouseParticles:GetComponentParticle():ResumeParticleSpawn()
					mouseParticles:GetTransform():SetPosition(destination)
					particleFlag = true
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
	bulletCount = maxBullets
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

	CreateBullet()
	bulletCount = bulletCount - 1
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

function SetBulletDirection(bullet)
	
	local playerPos2D = { componentTransform:GetPosition().x, componentTransform:GetPosition().z }
	local bulletPos2D = { bullet:GetTransform():GetPosition().x, bullet:GetTransform():GetPosition().z }
	local dist = Distance(playerPos2D, bulletPos2D)
	local dir = { bulletPos2D[1] - playerPos2D[1], bulletPos2D[2] - playerPos2D[2] }
	dir = Normalize(dir, dist)
	dir3 = float3.new(dir[1], 0, dir[2])
	
	bullet:GetTransform():SetFront(dir3)

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

print("Player.lua compiled succesfully")

-------- Scraps --------
--local components = gameObject:GetComponents()
--print(components[3].type) -- return it as an int, can't associate back to enum (I don't know how to anyway)

--GameState = require "Assets.Scripts.GameState"
--GameState:Update(1)
--print(GameState:GetGameState())