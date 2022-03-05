print("Bullet.lua loaded")

speed = 10  -- consider Start()

function Update(dt)

	local front = componentTransform:GetFront()
	componentTransform:SetPosition(float3.new(componentTransform:GetPosition().x + front.x * speed * dt, componentTransform:GetPosition().y + front.y * speed * dt, componentTransform:GetPosition().z + front.z * speed * dt))

	--Life Timer here
end

print("Bullet.lua compiled succesfully")