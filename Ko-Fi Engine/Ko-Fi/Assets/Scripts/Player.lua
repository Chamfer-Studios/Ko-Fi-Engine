------------------- Variables --------------------

State = {
   IDLE = 1,
   CROUCH = 2,
   PRONE = 3,
}

currentState = State.IDLE
speed = 25  -- consider Start()
isDoubleShot = false
bullets = 10


local speedIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT			-- IVT == Inspector Variable Type
speedIV = InspectorVariable.new("speed", speedIVT, speed)
NewVariable(speedIV)

local isDoubleShotIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL
isDoubleShotIV = InspectorVariable.new("isDoubleShot", isDoubleShotIVT, isDoubleShot)
NewVariable(isDoubleShotIV)


local currentItemType = ItemType.ITEM_GUN
currentItemDamage = 5
currentItem = Item.new(currentItemType, currentItemDamage)

-- Try to switch and use next

-------------------- Methods ---------------------
function Start()
	destination = nil
end

-- Called each loop iteration
function Update(dt)

	--mouseRight = GetInput(3)
	if (destination ~= nil)	then
		MoveToDestination(dt)
	end
	if (gameObject:IsSelected() == true)
		then --Gather Inputs
			if (GetInput(5) == KEY_STATE.KEY_DOWN) then
				currentItem.type = ItemType.ITEM_HAND
			end
			if (GetInput(6) == KEY_STATE.KEY_DOWN) then
				currentItem.type = ItemType.ITEM_KNIFE
			end
			if (GetInput(7) == KEY_STATE.KEY_DOWN) then
				currentItem.type = ItemType.ITEM_GUN
			end	
			if (GetInput(8) == KEY_STATE.KEY_DOWN) 
				then
					if (currentState == State.CROUCH) then
						currentState = State.IDLE
					else
						currentState = State.CROUCH
					end
			end	
			if (GetInput(9) == KEY_STATE.KEY_DOWN) 
				then
					if (currentState == State.PRONE) then
						currentState = State.IDLE
					else
						currentState = State.PRONE
					end	
			end
			if (GetInput(10) == KEY_STATE.KEY_DOWN) then
				Reload()
			end
			if (GetInput(4) == KEY_STATE.KEY_DOWN) 
				then
					if (currentItem.type == ItemType.ITEM_GUN and bullets > 0) then
						CreateBullet()
						bullets = bullets - 1
						if (isDoubleShot and bullets > 0) then
							CreateBullet()
							bullets = bullets - 1
						end
					elseif (currentItem.type == ItemType.ITEM_KNIFE) then
						print("Knife used")
					elseif (currentItem.type == ItemType.ITEM_NO_TYPE) then
						print("No item selected")
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

	if (d > 0.5)
		then 
			local s = speed
			if (currentState == State.CROUCH) then
				s = speed * 0.66
			elseif (currentState == State.PRONE) then
				s = speed * 0.33
			end

			-- Movement
			componentTransform:SetPosition(float3.new(componentTransform:GetPosition().x + (vec2[1] / d) * s * dt, componentTransform:GetPosition().y, componentTransform:GetPosition().z + (vec2[2] / d) * s * dt))

			-- Rotation
			vec2 = Normalize(vec2, d)
			local rad = math.acos(vec2[2])
			if(vec2[1] < 0)	then
				rad = rad * (-1)
			end
			componentTransform:SetRotation(float3.new(componentTransform:GetRotation().x, componentTransform:GetRotation().y, rad))
		else
			destination = nil
	end
end

function Reload()
	bullets = 10
end

-------------------- Setters ---------------------

function SetDestination(dest)
	destination = dest
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