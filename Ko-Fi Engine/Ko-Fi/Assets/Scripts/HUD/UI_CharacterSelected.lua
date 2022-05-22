characterID = 1

local characterIDIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
characterIDIV = InspectorVariable.new("characterID", characterIDIVT, characterID)
NewVariable(characterIDIV)

-- Called each loop iteration
function Update(dt)
	if (gameObject:GetButton():IsPressed() == true) then
		if (GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) == characterID) then
			--SetVariable(0, "GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
			DispatchGlobalEvent("Character_Selected", { 0 })
		elseif (GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) ~= characterID) then
			--SetVariable(characterID, "GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
			DispatchGlobalEvent("Character_Selected", { characterID })
		end
	end
end

print("UI_CharacterSelected.lua compiled succesfully")