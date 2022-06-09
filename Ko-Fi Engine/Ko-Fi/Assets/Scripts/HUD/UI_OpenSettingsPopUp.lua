name = "Settings Menu"
isStarting = true
popUp = false

local nameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
nameIV = InspectorVariable.new("name", nameIVT, name)
NewVariable(nameIV)

-- Called each loop iteration
function Update(dt)
	if (isStarting == true) then
		child = gameObject:GetParent():GetChild(name)
		child:Active(false)
		isStarting = false
	end
	if (gameObject:GetButton():IsPressed()) then
		if (popUp == false) then
			popUp = true
			child:Active(true)
			
		elseif (popUp == true) then
			popUp = false
			child:Active(false)
		end
	end
end

print("UI_OpenSettingsPopUp.lua compiled succesfully")