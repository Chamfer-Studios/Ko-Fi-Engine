print("Player.lua loaded")

------------------- Variables --------------------

speed = 3  -- consider Start()

bullets = {}

-------------------- Methods ---------------------
-- Called each loop iteration
function Update(dt)

	if(destination ~= nil)	then
		Move(dt)
	end
	
	mLeftButton = GetMouseButton(1)
	mRightButton = GetMouseButton(3)

	if(mRightButton == KEY_STATE.KEY_DOWN) then
		CreateBullet()
	end

end

-- Move to destination
function Move(dt)

	local targetPos = { destination.x, destination.z }
	local pos2D = { componentTransform:GetPosition().x, componentTransform:GetPosition().z }
	local d = Distance(pos2D, targetPos)

	if(d > 0.025)
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
	local dir = { bulletPos2D[1] - playerPos2D[1], bulletPos2D[2] - playerPos2D[2] }
	local dist = Distance(playerPos2D, bulletPos2D)
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