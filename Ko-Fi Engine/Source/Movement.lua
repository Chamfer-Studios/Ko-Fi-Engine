print("Movement.lua loaded")

-- Variables
local variable = {}
speed = 2
posX = 0
poxY = 0
posZ = 0
local targetPos = {}

-- Methods
function PreUpdate()
	
end

-- Called each loop iteration
function Update(dt, x, y, z, goTo_x, goTo_y, mouseLeftClick, mouseRightClick)

	pos = {x,z}

	posY = y
	posZ = z

	targetPos = {goTo_x, goTo_y}
	local d = Distance(pos, targetPos)
	print (d)
	if(d > 0.001)
	then --move
		local vec = { targetPos[1] - pos[1], targetPos[2] - pos[2] }
		posX = posX + (vec[1] / d) * speed * dt
		posZ = posZ + (vec[2] / d) * speed * dt
	end
	
	return 0
end

function PostUpdate()

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