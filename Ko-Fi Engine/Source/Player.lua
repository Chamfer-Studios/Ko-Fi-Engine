print("Player.lua loaded")

-- Variables
speed = 3  -- consider Start()

bullets = {}
--table.setn(bullets, 100)

i = 1

-- Methods:
-- Called each loop iteration
function Update(dt)

	if(destination ~= nil)	then
		Move(dt)
	end
	
	mLeftButton = GetMouseButton(1)
	mRightButton = GetMouseButton(3)

	if(mLeftButton == KEY_STATE.KEY_DOWN) then
		Fire()
	end

end

function PostUpdate(dt)
	
	return 
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

function Fire()

	print("Bang!")

	CreateBullet()
	--local bullet = CreateBullet()
	--bullet.name = "Bullet " ..i
	--i = i + 1
	--print(bullet.name)
	--table.insert(bullets, bullet)

	

end

function SetDestination(dest)
	destination = dest
end

function Distance(a, b)
    local dx, dy = a[1] - b[1], a[2] - b[2]
    return math.sqrt(dx * dx + dy * dy)
end

print("Player.lua compiled succesfully")