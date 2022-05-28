isStarting = true
-- Called each loop iteration
function Update(dt)
	if (gameObject.active == true) then
		if (isStarting == true) then
			vSync = GetVsync()
			gameObject:GetChild("cross").active = vSync
			isStarting = false
		end
		if (gameObject:GetButton():IsPressed() == true) then
			vSync = GetVsync()
			if (vSync == true) then
				SetVsync(false)
				gameObject:GetChild("cross").active = false
			elseif (vSync == false) then
				SetVsync(true)
				gameObject:GetChild("cross").active = true
			end
		end
	end
end

print("UI_Vsync.lua compiled succesfully")