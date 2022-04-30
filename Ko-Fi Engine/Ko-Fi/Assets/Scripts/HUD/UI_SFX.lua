-- player = Find("Character")


-- Called each loop iteration
function Update(dt)
	if (gameObject:GetButton():IsPressed() == true) then
		gameObject:GetAudioSwitch():PlayTrack(0)
	end
	if (gameObject:GetButton():IsHovered() == true) then
		gameObject:GetAudioSwitch():PlayTrack(1)
	end
end

print("UI_SFX.lua compiled succesfully")