print("Movement.lua loaded")

-- Variables
local variable = {}
local speed = 10

-- Methods
function PreUpdate()
	
end

-- Called each loop iteration
function Update(dt, x, y, z, mouseLeftClick, mouseRightClick)
	if(mouseLeftClick == 1 or mouseLeftClick == 2) -- 2 equals key_repeat
	then	
		x = x + speed * dt
	end

	if(mouseRightClick == 1 or mouseRightClick == 2) -- 2 equals key_repeat
	then	
		x = x - speed * dt
	end

	return x
end

function PostUpdate()

end

--function ToShowInPanel()
--	return {speed, 10}[2]
--end

print("Movement.lua compiled succesfully")