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
			image2 = checked
			gameObject:GetChild("30 Button"):GetButton():SetIdleTexture(image1)
			gameObject:GetChild("60 Button"):GetButton():SetIdleTexture(image2)
			gameObject:GetChild("120 Button"):GetButton():SetIdleTexture(image3)
			isStarting = false
		end
		if (gameObject:GetChild("30 Button"):GetButton():IsPressed() == true) then
			image1 = checked
			image2 = default
			image3 = default
			SetFPS(30)
			gameObject:GetChild("30 Button"):GetButton():SetIdleTexture(image1)
			gameObject:GetChild("60 Button"):GetButton():SetIdleTexture(image2)
			gameObject:GetChild("120 Button"):GetButton():SetIdleTexture(image3)
		elseif (gameObject:GetChild("60 Button"):GetButton():IsPressed() == true) then
			image1 = default
			image2 = checked
			image3 = default
			SetFPS(60)
			gameObject:GetChild("30 Button"):GetButton():SetIdleTexture(image1)
			gameObject:GetChild("60 Button"):GetButton():SetIdleTexture(image2)
			gameObject:GetChild("120 Button"):GetButton():SetIdleTexture(image3)
		elseif (gameObject:GetChild("120 Button"):GetButton():IsPressed() == true) then
			image1 = default
			image2 = default
			image3 = checked
			SetFPS(120)
			gameObject:GetChild("30 Button"):GetButton():SetIdleTexture(image1)
			gameObject:GetChild("60 Button"):GetButton():SetIdleTexture(image2)
			gameObject:GetChild("120 Button"):GetButton():SetIdleTexture(image3)
		end
	end
end

print("UI_FPS.lua compiled succesfully")