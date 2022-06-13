isStarting = true
default = "Assets/New UI/checkbox_a_default.png"
checked = "Assets/New UI/checkbox_a_checked.png"
image = default

-- Called each loop iteration
function UpdateUI(dt)
	if (gameObject.active == true) then
		if (isStarting == true) then
			borderless = GetBorderless()
			if (borderless == true) then
				image = checked
			elseif (fullscreen == false) then
				image = default
			end
			gameObject:GetButton():SetIdleTexture(image)
			isStarting = false
		end
		if (gameObject:GetButton():IsPressed() == true) then
			borderless = GetBorderless()
			if (borderless == true) then
				SetBorderless(false)
				image = default
			elseif (borderless == false) then
				SetBorderless(true)
				image = checked
			end
			gameObject:GetButton():SetIdleTexture(image)
		end
	end
end

print("UI_Borderless.lua compiled succesfully")