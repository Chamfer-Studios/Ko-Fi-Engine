isStarting = true
default = "Assets/New UI/checkbox_a_default.png"
checked = "Assets/New UI/checkbox_a_checked_1.png"
image = default

-- Called each loop iteration
function UpdateUI(dt)
	if (gameObject.active == true) then
		if (isStarting == true) then
			fullscreen = GetFullscreen()
			if (fullscreen == true) then
				image = checked
			elseif (fullscreen == false) then
				image = default
			end
			sb = Find("WSM SB 1")
			gameObject:GetButton():SetIdleTexture(image)
			isStarting = false
		end
		if (sb:GetButton():IsPressed() == true) then
			fullscreen = GetFullscreen()
			if (fullscreen == true) then
				SetFullscreen(false)
				image = default
			elseif (fullscreen == false) then
				SetFullscreen(true)
				image = checked
			end
			gameObject:GetButton():SetIdleTexture(image)
		end
	end
end

print("UI_Fullscreen.lua compiled succesfully")