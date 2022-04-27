characterID = 1

local characterIDIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
characterIDIV = InspectorVariable.new("characterID", characterIDIVT, characterID)
NewVariable(characterIDIV)

-- Called each loop iteration
function Update(dt)
	if (gameObject.active == true) then
		if (gameObject:GetButton():IsPressed() == true) then
			SetVariable(characterID, "UI_OoSpicy.lua", "CharacterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT);
		end
	end
end

print("UI_CharacterTS.lua compiled succesfully")