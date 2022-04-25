nameSquare = "Square"
nameText = "TextDescription"
isStarting = true

-- Called each loop iteration
function Update(dt)
	if (GetVariable("UI_OpenQuestsPopUp.lua", "popUp", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true) then
		if (isStarting == true) then
			child = gameObject:GetParent():GetChild(nameSquare):GetChild(nameText)
			child:Active(false)
			isStarting = false
		end
		if (gameObject.active == true) then
			if (gameObject:GetButton():IsPressed()) then
				child:Active(true)
			end
		end
	end
end

print("UI_Mission.lua compiled succesfully")