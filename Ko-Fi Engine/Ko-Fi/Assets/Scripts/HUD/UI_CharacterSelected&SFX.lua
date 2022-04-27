-- player = Find("Character")
characterID = 1

local characterIDIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
characterIDIV = InspectorVariable.new("characterID", characterIDIVT, characterID)
NewVariable(characterIDIV)

-- Called each loop iteration
function Update(dt)
	if (gameObject:GetButton():IsPressed() == true) then
		SetVariable(characterID, "GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT);
		gameObject:GetAudioSwitch():PlayTrack(0)
	end
	if (gameObject:GetButton():IsHovered() == true) then
		gameObject:GetAudioSwitch():PlayTrack(1)
	end
end

print("UI_CharacterSelected&SFX.lua compiled succesfully")