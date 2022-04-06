-- player = Find("Character")


-- Called each loop iteration
function Update(dt)
	if (gameObject:GetButton():IsPressed() == true) then
		SetVariable(1, "GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT);
		print("CharacterSelected")
	end
end

print("UI_CharacterSelected.lua compiled succesfully")