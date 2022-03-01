print("Movement.lua loaded")

-- Variables
speed = 3
--componentTransform = nil
local targetPos = {}

-- Methods
function PreUpdate()
	
end

-- Called each loop iteration
function Update(dt, goTo_x, goTo_y, mouseLeftClick, mouseRightClick)

	--pos = {x,z}
	--
	--posY = y
	--posZ = z
	--
	--targetPos = {goTo_x, goTo_y}
	--local d = Distance(pos, targetPos)
	--print (d)
	--if(d > 0.001)
	--then --move
	--	local vec = { targetPos[1] - pos[1], targetPos[2] - pos[2] }
	--	posX = posX + (vec[1] / d) * speed * dt
	--	posZ = posZ + (vec[2] / d) * speed * dt
	--end
	--
	--return

	

	targetPos = { goTo_x, goTo_y }
	pos2D = { componentTransform:GetPosition().x, componentTransform:GetPosition().z }
	local d = Distance(pos2D, targetPos)
	if(d > 0.025)
	then --move
		local vec2 = { targetPos[1] - pos2D[1], targetPos[2] - pos2D[2] }
		componentTransform:SetPosition(float3.new(componentTransform:GetPosition().x + (vec2[1] / d) * speed * dt, componentTransform:GetPosition().y, componentTransform:GetPosition().z + (vec2[2] / d) * speed * dt))
	end
	
	return
end

function PostUpdate(x, y)

end

--function ToShowInPanel()
--	return {speed, 10}[2]
--end

function Distance(a, b)
    local dx, dy = a[1] - b[1], a[2] - b[2]
    return math.sqrt(dx * dx + dy * dy)
end

--function VectorSub(a, b, d)
--	
--	return vec[1], vec[2]
--end

print("Movement.lua compiled succesfully")