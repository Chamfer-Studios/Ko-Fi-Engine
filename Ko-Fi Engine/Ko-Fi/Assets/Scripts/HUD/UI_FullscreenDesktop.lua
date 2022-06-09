isStarting = true
default = "Assets/New UI/checkbox_a_default.png"
checked = "Assets/New UI/checkbox_a_checked.png"
image = default

-- Called each loop iteration
function Update(dt)
	if (gameObject.active == true) then
		if (isStarting == true) then
			fullscreenDesktop = GetFullscreenDesktop()
			if (fullscreenDesktop == true) then
				image = checked
			elseif (fullscreenDesktop == false) then
				image = default
			end
			gameObject:GetButton():SetIdleTexture(image)
			isStarting = false
		end
		if (gameObject:GetButton():IsPressed() == true) then
			fullscreenDesktop = GetFullscreenDesktop()
			if (fullscreenDesktop == true) then
				SetFullscreenDesktop(false)
				image = default
			elseif (fullscreenDesktop == false) then
				SetFullscreenDesktop(true)
				image = checked
			end
			gameObject:GetButton():SetIdleTexture(image)
		end
	end
end

print("UI_FullscreenDesktop.lua compiled succesfully")