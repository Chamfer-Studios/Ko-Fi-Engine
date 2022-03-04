print("Player.lua loaded")

------------------- Variables --------------------

speed = 5  -- consider Start()

bullets = {}

-------------------- Methods ---------------------

-- Called each loop iteration
function Update(dt)

	mouseLeft = GetInput(1)
	mouseRight = GetInput(3)
	spaceButton = GetInput(8)
	goingRight = false
	-------------------------------- To be removed after VS --------------------------------
	local pos2D = { componentTransform:GetPosition().x, componentTransform:GetPosition().z }
	local targetPos = { componentTransform:GetPosition().x, componentTransform:GetPosition().z }
	if (GetInput(4) == KEY_STATE.KEY_DOWN or GetInput(4) == KEY_STATE.KEY_REPEAT) then
		targetPos[2] = targetPos[2] + 1
	end
	if (GetInput(5) == KEY_STATE.KEY_DOWN or GetInput(5) == KEY_STATE.KEY_REPEAT) then
		targetPos[1] = targetPos[1] + 1
	end
	if (GetInput(6) == KEY_STATE.KEY_DOWN or GetInput(6) == KEY_STATE.KEY_REPEAT) then
		targetPos[2] = targetPos[2] - 1
	end
	if (GetInput(7) == KEY_STATE.KEY_DOWN or GetInput(7) == KEY_STATE.KEY_REPEAT) then
		targetPos[1] = targetPos[1] - 1
		goingRight = true
	end
	if (pos2D[1] ~= targetPos[1] or pos2D[2] ~= targetPos[2]) then
		
		-- Move
		local d = Distance(pos2D, targetPos)
		local vec2 = { targetPos[1] - pos2D[1], targetPos[2] - pos2D[2] }
		
		vec2 = Normalize(vec2, d)
		componentTransform:SetPosition(float3.new(pos2D[1] + vec2[1] * speed * dt, componentTransform:GetPosition().y, pos2D[2] + vec2[2] * speed * dt))
		
		a = { 0, 1 } 
		rad = math.acos(a[1] * vec2[1] + a[2] * vec2[2])
		if (goingRight == true) then
			rad = rad * (-1)
		end

		componentTransform:SetRotation(float3.new(componentTransform:GetRotation().x, rad, componentTransform:GetRotation().z))
	end
	----------------------------------------------------------------------------------------

	if (destination ~= nil)	then
		MoveToDestination(dt)
	end

	if (spaceButton == KEY_STATE.KEY_DOWN) then
		CreateBullet()
	end

end

-- Move to destination
function MoveToDestination(dt)

	local targetPos = { destination.x, destination.z }
	local pos2D = { componentTransform:GetPosition().x, componentTransform:GetPosition().z }
	local d = Distance(pos2D, targetPos)

	if (d > 0.025)
		then --move
			local vec2 = { targetPos[1] - pos2D[1], targetPos[2] - pos2D[2] }
			componentTransform:SetPosition(float3.new(componentTransform:GetPosition().x + (vec2[1] / d) * speed * dt, componentTransform:GetPosition().y, componentTransform:GetPosition().z + (vec2[2] / d) * speed * dt))
		else
			destination = nil
	end

end

-------------------- Setters ---------------------

function SetDestination(dest)
	destination = dest
end

function SetTarget(tar)
	target = tar
end

function SetBulletDirection(bullet, parent)
	
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