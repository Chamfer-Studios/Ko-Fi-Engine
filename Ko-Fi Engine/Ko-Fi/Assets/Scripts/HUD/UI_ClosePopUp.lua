isStarting = true
stop = false
name = "UI_OpenPopUp.lua"

local nameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
nameIV = InspectorVariable.new("name", nameIVT, name)
NewVariable(nameIV)

-- Called each loop iteration
function Update(dt)
	if (isStarting == true) then
		child = gameObject:GetParent()
		isStarting = false
	end
	if (gameObject:GetButton():IsPressed()) then
		if (gameObject.active == true) then
			child:Active(false)
			startTime = os.time()
			endTime = startTime+1
			stop = true
		end
	end
	if (stop == true) then
		if (os.time() >= endTime) then
			SetVariable(false, name, "popUp", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL)
			stop = false
		end
	end
end

print("UI_ClosePopUp.lua compiled succesfully")