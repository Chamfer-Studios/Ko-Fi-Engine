-- player = Find("Character")
name = "Level_1"

local nameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
nameIV = InspectorVariable.new("name", nameIVT, name)
NewVariable(nameIV)

-- Called each loop iteration
function Update(dt)
	if (gameObject:GetParent():GetChild("SkillsMenu").active == false) then
		if (gameObject:GetButton():IsPressed() == true) then
			-- gameObject:LoadScene("HUD_Scene")
			gameObject:ChangeScene(true, name);
		end
	end
end

print("UI_ChangeScenes.lua compiled succesfully")