-- player = Find("Character")
characterID = 1

local characterIDIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
characterIDIV = InspectorVariable.new("lives", characterIDIVT, characterID)
NewVariable(characterIDIV)

-- Called each loop iteration
function Update(dt)
	if (gameObject:GetButton():IsPressed() == true) then
		SetVariable(characterID, "GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT);
		print("CharacterSelected")
	end
end

print("UI_CharacterSelected.lua compiled succesfully")