-- Variables Publicas
-- Angulo de Rotacion, Velocidad de Rotacion

rotationAngle = 90
rotationSpeed = 40

local rotationAngleIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT			-- IVT == Inspector Variable Type
rotationAngleIV = InspectorVariable.new("rotationAngle", rotationAngleIVT, rotationAngle)
NewVariable(rotationAngleIV)

local rotationSpeedIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT			-- IVT == Inspector Variable Type
rotationSpeedIV = InspectorVariable.new("rotationSpeed", rotationSpeedIVT, rotationSpeed)
NewVariable(rotationSpeedIV)

target = nil
local targetIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_GAMEOBJECT			-- IVT == Inspector Variable Type
targetIV = InspectorVariable.new("target", targetIVT, target)
NewVariable(targetIV)

viewOffset = float3.new(0, 0, 0)
--local viewOffsetIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT3			-- IVT == Inspector Variable Type
--viewOffsetIV = InspectorVariable.new("viewOffset", viewOffsetIVT, viewOffset)
--NewVariable(viewOffsetIV)

function Float3Length(v)
    return math.sqrt(v.x * v.x + v.y * v.y + v.z * v.z)
end

function Float3Normalized(v)
    len = Float3Length(v)
    return { x = v.x / len, y = v.y / len, z = v.z / len }
end

function Float3Difference(a, b)
    return { x = b.x - a.x, y = b.y - a.y, z = b.z - a.z }
end

function Float3Distance(a, b)
    diff = Float3Difference(a, b)
    return Float3Length(diff)
end

function Float3NormalizedDifference(a, b)
    v = Float3Difference(a, b)
    return Float3Normalized(v)
end

function Float3Dot(a, b)
    return a.x * b.x + a.y * b.y + a.z * b.z
end

function Float3Angle(a, b)
    lenA = Float3Length(a)
    lenB = Float3Length(b)

    return math.acos(Float3Dot(a, b) / (lenA * lenB))
end

transform = gameObject:GetTransform()

-------------------- Methods ---------------------
function GetCameraTargetAngle()
	diff = Float3Difference(target:GetTransform():GetPosition(), transform:GetPosition())
	diff.y = 0
	return Float3Angle(diff, viewOffset)
end

direction = 0

function Start()
	viewOffset = Float3Difference(target:GetTransform():GetPosition(), transform:GetPosition())
	viewOffset = Float3Normalized(viewOffset)
	direction = 1
end

changedDirection = false

function Update(dt)
	if direction ~= 0 then
		targetTransform = target:GetTransform()
		
		deltaA = direction * dt * rotationSpeed

		if changedDirection == false then
			deltaA = 0
		end

		RotateAround(targetTransform:GetPosition(), deltaA)

		if GetCameraTargetAngle() > math.rad(rotationAngle / 2) then
			changedDirection = false
		end

		--Log(tostring(math.deg(GetCameraTargetAngle())) .. "\n")
		--if math.deg(GetCameraTargetAngle()) < rotationAngle / 2 then
		--end
		--end
	end
	gameObject:GetCamera():LookAt(targetTransformiu:GetPosition())
	
	if (GetInput(14) == KEY_STATE.KEY_DOWN) then -- Q
		if direction ~= -1 then
			direction = -1
			changedDirection = true
		end
	end
	if (GetInput(15) == KEY_STATE.KEY_DOWN) then -- E
		if direction ~= 1 then
			direction = 1
			changedDirection = true
		end
	end
	
end

function RotateAround(center, angle)

	local position = float3.new(0,0,0)
	position = transform:GetPosition();

    -- Get the desired rotation
	radAngle = math.rad(angle)
	local rotation = Quat.new(0,0,0,0)
	rotation = Quat.RotateY(radAngle)

    -- Find current direction relative to center
	local direction = float3.new(0,0,0)
	direction.x = position.x - center.x
	direction.y = position.y - center.y
	direction.z = position.z - center.z

	direction = MulQuat(rotation, direction) -- rotate the direction

	-- Define new position
	position.x = center.x + direction.x
	position.y = center.y + direction.y 
	position.z = center.z + direction.z 

	gameObject:GetTransform():SetPosition(position)


end

--------------------------------------------------