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
			image2 = checked
			pos1 = Find("30 Button")
			pos2 = Find("60 Button")
			pos3 = Find("120 Button")
			sb1 = Find("FPS SB 1")
			sb2 = Find("FPS SB 2")
			sb3 = Find("FPS SB 3")
			pos1:GetButton():SetIdleTexture(image1)
			pos2:GetButton():SetIdleTexture(image2)
			pos3:GetButton():SetIdleTexture(image3)
			isStarting = false
		end
		if (sb1:GetButton():IsPressed() == true) then
			image1 = checked
			image2 = default
			image3 = default
			SetFPS(30)
			pos1:GetButton():SetIdleTexture(image1)
			pos2:GetButton():SetIdleTexture(image2)
			pos3:GetButton():SetIdleTexture(image3)
		elseif (sb2:GetButton():IsPressed() == true) then
			image1 = default
			image2 = checked
			image3 = default
			SetFPS(60)
			pos1:GetButton():SetIdleTexture(image1)
			pos2:GetButton():SetIdleTexture(image2)
			pos3:GetButton():SetIdleTexture(image3)
		elseif (sb3:GetButton():IsPressed() == true) then
			image1 = default
			image2 = default
			image3 = checked
			SetFPS(120)
			pos1:GetButton():SetIdleTexture(image1)
			pos2:GetButton():SetIdleTexture(image2)
			pos3:GetButton():SetIdleTexture(image3)
		end
	end
end

print("UI_FPS.lua compiled succesfully")