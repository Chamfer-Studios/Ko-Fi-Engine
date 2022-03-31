------------------- Variables --------------------

speed = 300
life = 50
destination = nil

-------------------- Methods ---------------------

-- Called each loop iteration
function Update(dt)

	life = life - dt
	if (life <= 0) then
		DeleteGameObject()
	elseif (destination ~= nil) then
		MoveToDestination(dt)
	end
end

function PostUpdate(dt)

end

--------------------------------------------------

-- Move to destination
function MoveToDestination(dt)

	local targetPos2D = { destination.x, destination.z }
	local pos2D = { componentTransform:GetPosition().x, componentTransform:GetPosition().z }
	local d = Distance(pos2D, targetPos2D)
	local vec2 = { targetPos2D[1] - pos2D[1], targetPos2D[2] - pos2D[2] }

	if (d > 0.5)
		then
			-- Movement
			vec2 = Normalize(vec2, d)
			componentTransform:SetPosition(float3.new(componentTransform:GetPosition().x + vec2[1] * speed * dt, componentTransform:GetPosition().y, componentTransform:GetPosition().z + vec2[2] * speed * dt))

			if (componentTransform:GetPosition().x > destination.x) then
				componentTransform:SetPosition(float3.new(destination.x, componentTransform:GetPosition().y, componentTransform:GetPosition().z))
			end

			if (componentTransform:GetPosition().z > destination.z) then
				componentTransform:SetPosition(float3.new(componentTransform:GetPosition().x, componentTransform:GetPosition().y, destination.z))
			end

			-- Rotation
			local rad = math.acos(vec2[2])
			if(vec2[1] < 0)	then
				rad = rad * (-1)
			end
			componentTransform:SetRotation(float3.new(componentTransform:GetRotation().x, componentTransform:GetRotation().y, rad))
		else
			destination = nil
	end
end

function SetDestination()
	if (target ~= nil) then
		destination = target:GetTransform():GetPosition()
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