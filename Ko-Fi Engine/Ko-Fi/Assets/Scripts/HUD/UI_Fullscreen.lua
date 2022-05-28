isStarting = true
-- Called each loop iteration
function Update(dt)
	if (gameObject.active == true) then
		if (isStarting == true) then
			fullscreen = GetFullscreen()
			gameObject:GetChild("cross1").active = vSync
			isStarting = false
		end
		if (gameObject:GetButton():IsPressed() == true) then
			fullscreen = GetFullscreen()
			if (fullscreen == true) then
				SetFullscreen(false)
				gameObject:GetChild("cross1").active = false
			elseif (fullscreen == false) then
				SetFullscreen(true)
				gameObject:GetChild("cross1").active = true
			end
		end
	end
end

print("UI_Fullscreen.lua compiled succesfully")