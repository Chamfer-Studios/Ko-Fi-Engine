name = "Level_1"

local nameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
nameIV = InspectorVariable.new("name", nameIVT, name)
NewVariable(nameIV)

-- Called each loop iteration
function Update(dt)
	if (gameObject:GetButton():IsPressed() == true) then
		gameObject:ChangeScene(true, name);
		gameObject:GetAudioSwitch():PlayTrack(0)
	end
	if (gameObject:GetButton():IsHovered() == true) then
		gameObject:GetAudioSwitch():PlayTrack(1)
	end
end

print("UI_ChangeScenes&SFX.lua compiled succesfully")