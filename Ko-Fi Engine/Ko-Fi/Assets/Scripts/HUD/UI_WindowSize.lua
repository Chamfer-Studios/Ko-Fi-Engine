isStarting = true
default = "Assets/New UI/checkbox_a_default.png"
checked = "Assets/New UI/checkbox_a_checked_1.png"
image1 = default
image2 = default
image3 = default

-- Called each loop iteration
function UpdateUI(dt)
	if (gameObject.active == true) then
		if (isStarting == true) then
			pos1 = Find("WS Button 1")
			pos2 = Find("WS Button 2")
			pos3 = Find("WS Button 3")
			sb1 = Find("WS SB 1")
			sb2 = Find("WS SB 2")
			sb3 = Find("WS SB 3")
			isStarting = false
		end
		if (sb1:GetButton():IsPressed() == true) then
			image1 = checked
			image2 = default
			image3 = default
			SetWidth(1024)
			SetHeight(768)
			pos1:GetButton():SetIdleTexture(image1)
			pos2:GetButton():SetIdleTexture(image2)
			pos3:GetButton():SetIdleTexture(image3)
		elseif (sb2:GetButton():IsPressed() == true) then
			image1 = default
			image2 = checked
			image3 = default
			SetWidth(960)
			SetHeight(540)
			pos1:GetButton():SetIdleTexture(image1)
			pos2:GetButton():SetIdleTexture(image2)
			pos3:GetButton():SetIdleTexture(image3)
		elseif (sb3:GetButton():IsPressed() == true) then
			image1 = default
			image2 = default
			image3 = checked
			SetWidth(1280)
			SetHeight(720)
			pos1:GetButton():SetIdleTexture(image1)
			pos2:GetButton():SetIdleTexture(image2)
			pos3:GetButton():SetIdleTexture(image3)
		end
	end
end

print("UI_WindowSize.lua compiled succesfully")