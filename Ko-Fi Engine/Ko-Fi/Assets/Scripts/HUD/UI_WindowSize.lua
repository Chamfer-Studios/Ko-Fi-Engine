isStarting = true
default = "Assets/New UI/checkbox_a_default.png"
checked = "Assets/New UI/checkbox_a_checked.png"
image1 = default
image2 = default
image3 = default

-- Called each loop iteration
function Update(dt)
	if (gameObject.active == true) then
		if (isStarting == true) then
			pos1 = Find("WS Button 1")
			pos2 = Find("WS Button 2")
			pos3 = Find("WS Button 3")
			isStarting = false
		end
		if (pos1:GetButton():IsPressed() == true) then
			image1 = checked
			image2 = default
			image3 = default
			SetWidth(1024)
			SetHeight(768)
			pos1:GetButton():SetIdleTexture(image1)
			pos2:GetButton():SetIdleTexture(image2)
			pos3:GetButton():SetIdleTexture(image3)
		elseif (pos2:GetButton():IsPressed() == true) then
			image1 = default
			image2 = checked
			image3 = default
			SetWidth(1920)
			SetHeight(1080)
			pos1:GetButton():SetIdleTexture(image1)
			pos2:GetButton():SetIdleTexture(image2)
			pos3:GetButton():SetIdleTexture(image3)
		elseif (pos3:GetButton():IsPressed() == true) then
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

print("UI_FPS.lua compiled succesfully")