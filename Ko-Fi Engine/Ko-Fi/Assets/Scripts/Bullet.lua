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