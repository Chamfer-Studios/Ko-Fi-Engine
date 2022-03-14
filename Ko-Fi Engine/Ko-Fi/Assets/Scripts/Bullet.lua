------------------- Variables --------------------

speed = 300
life = 0.5

-------------------- Methods ---------------------

function Start()
	
	-- This is dumb :/ No wonder it didn't work lol
	-- The bullet spawns in front so it never really changes direction with this


	local playerPos2D = { gameObject:GetParent():GetTransform():GetPosition().x, gameObject:GetParent():GetTransform():GetPosition().z }
	local bulletPos2D = { componentTransform:GetPosition().x, componentTransform:GetPosition().z }
	local dist = Distance(playerPos2D, bulletPos2D)
	local dir = { bulletPos2D[1] - playerPos2D[1], bulletPos2D[2] - playerPos2D[2] }
	dir = Normalize(dir, dist)

	local dir3 = float3.new(dir[1], 0.0, dir[2])

	local front = componentTransform:GetFront()
	--print(front.x, front.y, front.z)	
	
	componentTransform:SetFront(dir3)
end

function Update(dt)

	life = life - dt
	if (life <= 0) then
		DeleteGameObject()
	else
		local front = componentTransform:GetFront()
		componentTransform:SetPosition(float3.new(componentTransform:GetPosition().x + front.x * speed * dt, componentTransform:GetPosition().y + front.y * speed * dt, componentTransform:GetPosition().z + front.z * speed * dt))
	end

end

--------------------------------------------------

function Normalize(vec, distance)
    
	vec[1] = vec[1] / distance
	vec[2] = vec[2] / distance

	return vec
end

function Distance(a, b)

    local dx, dy = a[1] - b[1], a[2] - b[2]
    return math.sqrt(dx * dx + dy * dy)

end