rightPos = 500
leftPos = 0
isActive = false
screenBrightness = 0
isStarting = true

local rightPosIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
rightPosIV = InspectorVariable.new("rightPos", rightPosIVT, rightPos)
NewVariable(rightPosIV)

local leftPosIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
leftPosIV = InspectorVariable.new("leftPos", leftPosIVT, leftPos)
NewVariable(leftPosIV)

-- Called each loop iteration
function Update(dt)
	if (gameObject.active == true) then
		if (isStarting == true) then
			screenBrightness = GetBrightness()
			screenBrightness = screenBrightness * 4
			if (screenBrightness == 0.0) then
				mouseX = leftPos
			elseif (screenBrightness == 1.0) then
				mouseX = leftPos + onePart
			elseif (screenBrightness == 2.0) then
				mouseX = leftPos + (onePart * 2)
			elseif (screenBrightness == 3.0) then
				mouseX = leftPos + (onePart * 3)
			elseif (screenBrightness == 4.0) then
				mouseX = rightPos
			end
			gameObject:GetTransform2D():SetPositionX(mouseX)
			isStarting = false
		end
		mouseX = gameObject:GetButton():GetMouseX()
		mouseX = mouseX * 5.3
		onePart = (rightPos - leftPos) / 4
		if (gameObject:GetButton():IsPressed() == true) then
			isActive = true
		end
		if (GetInput(1) == KEY_STATE.KEY_UP) then
			isActive = false
		end
		if (isActive == true) then
			if (mouseX < leftPos) then
				mouseX = leftPos
				gameObject:GetTransform2D():SetPositionX(mouseX)
				SetBrightness(0.0)
			elseif (mouseX >= (leftPos + onePart) and mouseX <= (leftPos + (onePart * 2))) then
				mouseX = leftPos + onePart
				gameObject:GetTransform2D():SetPositionX(mouseX)
				SetBrightness(0.25)
			elseif (mouseX >= (leftPos + (onePart * 2)) and (mouseX <= (leftPos + (onePart * 3)))) then
				mouseX = leftPos + (onePart * 2)
				gameObject:GetTransform2D():SetPositionX(mouseX)
				SetBrightness(0.5)
			elseif (mouseX >= (leftPos + (onePart * 3)) and (mouseX < rightPos)) then
				mouseX = leftPos + (onePart * 3)
				gameObject:GetTransform2D():SetPositionX(mouseX)
				SetBrightness(0.75)
			elseif (mouseX > rightPos) then
				mouseX = rightPos
				gameObject:GetTransform2D():SetPositionX(mouseX)
				SetBrightness(1.0)
			end
		end
	end
end

print("UI_ScreenBrightness.lua compiled succesfully")