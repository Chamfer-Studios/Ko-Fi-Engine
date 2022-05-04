isCredits = false

local isCreditsIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL
isCreditsIV = InspectorVariable.new("isCredits", isCreditsIVT, isCredits)
NewVariable(isCreditsIV)

-- Called each loop iteration
function Update(dt)
	if (GetVariable("UI_OpenCreditsPopUp.lua", "popUp", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == isCredits) then
		if (gameObject:GetButton():IsPressed() == true) then
			gameObject:GetAudioSwitch():PlayTrack(0)
		end
		if (gameObject:GetButton():IsHovered() == true) then
			gameObject:GetAudioSwitch():PlayTrack(1)
		end
	end
end

print("UI_SFXMMS.lua compiled succesfully")