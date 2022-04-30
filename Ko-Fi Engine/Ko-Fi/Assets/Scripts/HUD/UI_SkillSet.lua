name1 = "ZhibSkills"
name2 = "NeralaSkills"
name3 = "ElOtroSkills"

--local nameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL
--nameIV = InspectorVariable.new("name", nameIVT, name)
--NewVariable(nameIV)

-- Called each loop iteration
function Update(dt)
	if (gameObject.active == true) then
		character = GetVariable("UI_OoSpicy.lua", "CharacterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
		if (character == 1) then
			gameObject:GetChild(name1):Active(true)
			gameObject:GetChild(name2):Active(false)
			gameObject:GetChild(name3):Active(false)
		end
		if (character == 2) then
			gameObject:GetChild(name1):Active(false)
			gameObject:GetChild(name2):Active(true)
			gameObject:GetChild(name3):Active(false)
		end
		if (character == 3) then
			gameObject:GetChild(name1):Active(false)
			gameObject:GetChild(name2):Active(false)
			gameObject:GetChild(name3):Active(true)
		end
	end
end

print("UI_SkillSet.lua compiled succesfully")