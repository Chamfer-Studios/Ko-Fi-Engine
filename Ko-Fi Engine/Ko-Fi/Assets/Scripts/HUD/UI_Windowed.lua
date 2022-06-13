isStarting = true
default = "Assets/New UI/checkbox_a_default.png"
checked = "Assets/New UI/checkbox_a_checked.png"
image = default

-- Called each loop iteration
function UpdateUI(dt)
	if (gameObject.active == true) then
		if (isStarting == true) then
			windowed = GetResizable()
			if (windowed == true) then
				image = checked
			elseif (windowed == false) then
				image = default
			end
			gameObject:GetButton():SetIdleTexture(image)
			isStarting = false
		end
		if (gameObject:GetButton():IsPressed() == true) then
			windowed = GetResizable()
			if (windowed == true) then
				SetResizable(false)
				image = default
			elseif (windowed == false) then
				SetResizable(true)
				image = checked
			end
			gameObject:GetButton():SetIdleTexture(image)
		end
	end
end

print("UI_Windowed.lua compiled succesfully")