------------------- Variables --------------------

speed = 300
life = 0.5

-------------------- Methods ---------------------

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