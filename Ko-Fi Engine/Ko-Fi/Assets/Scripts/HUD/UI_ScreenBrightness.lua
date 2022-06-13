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
function UpdateUI(dt)
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
			pos1 = Find("SB1")
			pos2 = Find("SB2")
			pos3 = Find("SB3")
			pos4 = Find("SB4")
			pos5 = Find("SB5")
			isStarting = false
		end
		onePart = (rightPos - leftPos) / 4
		if (gameObject:GetButton():IsPressed() == true) then
			isActive = true
		end
		if (GetInput(1) == KEY_STATE.KEY_UP) then
			isActive = false
		end
		if (isActive == true) then
			if (pos1:GetButton():IsPressed() == true) then
				mouseX = leftPos
				gameObject:GetTransform2D():SetPositionX(mouseX)
				SetBrightness(0.0)
			elseif (pos2:GetButton():IsPressed() == true) then
				mouseX = leftPos + onePart
				gameObject:GetTransform2D():SetPositionX(mouseX)
				SetBrightness(0.25)
			elseif (pos3:GetButton():IsPressed() == true) then
				mouseX = leftPos + (onePart * 2)
				gameObject:GetTransform2D():SetPositionX(mouseX)
				SetBrightness(0.5)
			elseif (pos4:GetButton():IsPressed() == true) then
				mouseX = leftPos + (onePart * 3)
				gameObject:GetTransform2D():SetPositionX(mouseX)
				SetBrightness(0.75)
			elseif (pos5:GetButton():IsPressed() == true) then
				mouseX = rightPos
				gameObject:GetTransform2D():SetPositionX(mouseX)
				SetBrightness(1.0)
			end
		end
	end
end

print("UI_ScreenBrightness.lua compiled succesfully")