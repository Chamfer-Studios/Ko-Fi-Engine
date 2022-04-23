-- player = Find("Character")
name = "Level_1"

local nameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
nameIV = InspectorVariable.new("name", nameIVT, name)
NewVariable(nameIV)

-- Called each loop iteration
function Update(dt)
	if (GetVariable("UI_OpenSkillsPopUp.lua", "popUp", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == false) then
		if (GetVariable("UI_OpenQuestsPopUp.lua", "popUp", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == false) then
			if (gameObject:GetButton():IsPressed() == true) then
				gameObject:ChangeScene(true, name);
			end
		end
	end
end

print("UI_ChangeScenes.lua compiled succesfully")