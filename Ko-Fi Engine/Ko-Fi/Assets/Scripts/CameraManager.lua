-- Variables Publicas
-- Radio, Angulo de Rotacion, Altura de la Camara

rotationAngle = 90
rotationSpeed = 2

local rotationAngleIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT			-- IVT == Inspector Variable Type
rotationAngleIV = InspectorVariable.new("rotationAngle", rotationAngleIVT, rotationAngle)
NewVariable(rotationAngleIV)

local rotationSpeedIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT			-- IVT == Inspector Variable Type
rotationSpeedIV = InspectorVariable.new("rotationSpeed", rotationSpeedIVT, rotationSpeed)
NewVariable(rotationSpeedIV)

transform = gameObject:GetTransform()


degToRad = 0.0174532925199432957
targetTime = 5
speed = 2
start = false
counter = targetTime
-------------------- Methods ---------------------
function Update(dt)
	--print(rotationAngle)

	buttonLeft = Find("ButtonL"):GetButton():GetButtonState()
	buttonRigth = Find("ButtonR"):GetButton():GetButtonState()
	target = Find("nerala_low"):GetTransform():GetPosition()

	if(start) then
		
		restAngle = (dt * rotationAngle / targetTime) -- Angle to Sum in each frame

		if(counter <= 0.00) then
			start = false
			print("set false")
		end
		
		RotateAround(target, restAngle)

		counter = counter - dt 
	end
		gameObject:GetCamera():LookAt(target)
	
	if (GetInput(5) == KEY_STATE.KEY_DOWN) then -- E
		if(rotationAngle > 0) then
		rotationAngle = -rotationAngle
		end
		start = true
		counter = targetTime
	end
		if (GetInput(14) == KEY_STATE.KEY_DOWN) then -- Q
		if(rotationAngle < 0) then
		rotationAngle = rotationAngle * -1
		end
		start = true
		counter = targetTime
	end
	
end

function RotateAround(center, angle)

	local position = float3.new(0,0,0)
	position = transform:GetPosition();
	a = angle * degToRad
	print(a)
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