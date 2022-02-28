print("Movement.lua loaded")

-- dt = float dt, xyz = pos.xyz, b = leftMouseClick, c = RightMouseClick
function Update(dt, x, y, z, b, c)
	if(b) 
	then	
		x = x + 0.1 --*dt
	end

	if(c) 
	then	
		x = x - 0.1 --*dt
	end

	return x
end

print("Movement.lua compiled")