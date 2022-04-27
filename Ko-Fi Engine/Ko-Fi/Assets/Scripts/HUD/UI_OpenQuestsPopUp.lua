name = "QuestsMenu"
isStarting = true
popUp = false

-- Called each loop iteration
function Update(dt)
	if (isStarting == true) then
		child = gameObject:GetParent():GetChild(name)
		child:Active(false)
		isStarting = false
	end
	if (GetVariable("UI_OpenSkillsPopUp.lua", "popUp", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == false) then
		if (popUp == false) then
			if (gameObject:GetButton():IsPressed()) then
				popUp = true
				child:Active(true)
			end
		end
	end
end

print("UI_OpenQuestsPopUp.lua compiled succesfully")