isStarting = true
default = "Assets/New UI/checkbox_a_default.png"
checked = "Assets/New UI/checkbox_a_checked_1.png"
image = default

-- Called each loop iteration
function UpdateUI(dt)
	if (gameObject.active == true) then
		if (isStarting == true) then
			vSync = GetVsync()
			if (vSync == true) then
				image = checked
			elseif (vSync == false) then
				image = default
			end
			gameObject:GetButton():SetIdleTexture(image)
			isStarting = false
		end
		if (gameObject:GetButton():IsPressed() == true) then
			vSync = GetVsync()
			if (vSync == true) then
				SetVsync(false)
				image = default
			elseif (vSync == false) then
				SetVsync(true)
				image = checked
			end
			gameObject:GetButton():SetIdleTexture(image)
		end
	end
end

print("UI_Vsync.lua compiled succesfully")