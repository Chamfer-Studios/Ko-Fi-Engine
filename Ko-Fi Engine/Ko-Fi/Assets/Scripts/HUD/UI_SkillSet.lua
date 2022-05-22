name1 = "ZhibSkills"
name2 = "NeralaSkills"
name3 = "OmozraSkills"
once = true
neri = true
omo = true
--local nameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL
--nameIV = InspectorVariable.new("name", nameIVT, name)
--NewVariable(nameIV)

-- Called each loop iteration
function Update(dt)
	if (gameObject.active == true) then
		character = GetVariable("UI_OoSpicy.lua", "CharacterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
		if (character == 1) then
			if (once == true) then
				gameObject:GetChild(name1):Active(true)
				gameObject:GetChild(name2):Active(false)
				gameObject:GetChild(name3):Active(false)
				once = false
				neri = true
				omo = true
			end
		end
		if (character == 2) then
			if (neri == true) then
				gameObject:GetChild(name1):Active(false)
				gameObject:GetChild(name2):Active(true)
				gameObject:GetChild(name3):Active(false)
				once = true
				neri = false
				omo = true
			end
		end
		if (character == 3) then
			if (omo == true) then
				gameObject:GetChild(name1):Active(false)
				gameObject:GetChild(name2):Active(false)
				gameObject:GetChild(name3):Active(true)
				once = true
				neri = true
				omo = false
			end
		end
	end
end

print("UI_SkillSet.lua compiled succesfully")