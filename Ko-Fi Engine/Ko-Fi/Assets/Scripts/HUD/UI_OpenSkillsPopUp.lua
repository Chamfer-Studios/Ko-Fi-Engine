name = "SkillsMenu"
isStarting = true
popUp = false

--local nameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
--nameIV = InspectorVariable.new("name", nameIVT, name)
--NewVariable(nameIV)

-- Called each loop iteration
function Update(dt)
	if (isStarting == true) then
		child = gameObject:GetParent():GetChild(name)
		child:Active(false)
		isStarting = false
	end
	if (popUp == false) then
		if (gameObject:GetButton():IsPressed()) then
			popUp = true
			child:Active(true)
		end
	end
end

print("UI_OpenSkillsPopUp.lua compiled succesfully")