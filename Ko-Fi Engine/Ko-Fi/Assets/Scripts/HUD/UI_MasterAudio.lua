rightPos = 500
leftPos = 0
isActive = false
masterVolume = 0
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
			audio = GetAudio()
			masterVolume = audio:GetListenerVolume()
			masterVolume = masterVolume * 4
			if (masterVolume == 0.0) then
				mouseX = leftPos
			elseif (masterVolume == 1.0) then
				mouseX = leftPos + onePart
			elseif (masterVolume == 2.0) then
				mouseX = leftPos + (onePart * 2)
			elseif (masterVolume == 3.0) then
				mouseX = leftPos + (onePart * 3)
			elseif (masterVolume == 4.0) then
				mouseX = rightPos
			end
			gameObject:GetTransform2D():SetPositionX(mouseX)
			pos1 = Find("MV1")
			pos2 = Find("MV2")
			pos3 = Find("MV3")
			pos4 = Find("MV4")
			pos5 = Find("MV5")
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
				audio:SetListenerVolume(0, 4)
			elseif (pos2:GetButton():IsPressed() == true) then
				mouseX = leftPos + onePart
				gameObject:GetTransform2D():SetPositionX(mouseX)
				audio:SetListenerVolume(1, 4)
			elseif (pos3:GetButton():IsPressed() == true) then
				mouseX = leftPos + (onePart * 2)
				gameObject:GetTransform2D():SetPositionX(mouseX)
				audio:SetListenerVolume(2, 4)
			elseif (pos4:GetButton():IsPressed() == true) then
				mouseX = leftPos + (onePart * 3)
				gameObject:GetTransform2D():SetPositionX(mouseX)
				audio:SetListenerVolume(3, 4)
			elseif (pos5:GetButton():IsPressed() == true) then
				mouseX = rightPos
				gameObject:GetTransform2D():SetPositionX(mouseX)
				audio:SetListenerVolume(4, 4)
			end
		end
	end
end

print("UI_MasterAudio.lua compiled succesfully")