-- Variables Publicas
-- Radio, Angulo de Rotacion, Altura de la Camara

rotationAngle =  124
--rotationSpeed = 2 --Not Working For Now

local rotationAngleIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT			-- IVT == Inspector Variable Type
rotationAngleIV = InspectorVariable.new("rotationAngle", rotationAngleIVT, rotationAngle)
NewVariable(rotationAngleIV)

--local rotationSpeedIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT			-- IVT == Inspector Variable Type
--rotationSpeedIV = InspectorVariable.new("rotationSpeed", rotationSpeedIVT, rotationSpeed)
--NewVariable(rotationSpeedIV)

transform = gameObject:GetTransform()


degToRad = 0.0174532925199432957
targetTime = 2
speed = 2
start = false
counter = targetTime
-------------------- Methods ---------------------
function Update(dt)
	--print(rotationAngle)

	--buttonLeft = Find("ButtonL"):GetButton():GetButtonState()
	--buttonRigth = Find("ButtonR"):GetButton():GetButtonState()
	id = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)

	if (id == 2) then	
	target = Find("nerala_low"):GetTransform():GetPosition()
	elseif (id == 1) then	
	target = Find("Zhib"):GetTransform():GetPosition()
	else
	target = 0
	end

	
	
	if(start) then
		
		restAngle = (dt * rotationAngle / targetTime) -- Angle to Sum in each frame

		if(counter <= 0.00) then
			start = false
		end
		if(target ~= 0) then
		RotateAround(target, restAngle)
		end
		counter = counter - dt 
	end
	if(target ~=0) then
		gameObject:GetCamera():LookAt(target)
	end
	if (GetInput(14) == KEY_STATE.KEY_DOWN) then -- H

		if(rotationAngle > 0) then
		rotationAngle = -rotationAngle
		end
		start = true
		counter = targetTime
	end

	if (GetInput(15) == KEY_STATE.KEY_DOWN) then -- K

		if(rotationAngle < 0) then
		rotationAngle = rotationAngle * -1
		end
		start = true
		counter = targetTime
	end
	
end

function RotateAround(center, angle)

	local position = float3.new(0,0,0)
	position = transform:GetPosition()
	a = angle * degToRad
	
	local rotation = Quat.new(0,0,0,0) -- get the desired rotation
	rotation = Quat.RotateY(a)
	
	local direction = float3.new(0,0,0) -- find current direction relative to center
	direction.x = position.x - center.x
	direction.y = position.y - center.y
	direction.z = position.z - center.z

	direction = MulQuat(rotation, direction) -- rotate the direction

	-- define new position
	position.x = center.x + direction.x
	position.y = center.y + direction.y 
	position.z = center.z + direction.z 

	gameObject:GetTransform():SetPosition(position)


end

--------------------------------------------------