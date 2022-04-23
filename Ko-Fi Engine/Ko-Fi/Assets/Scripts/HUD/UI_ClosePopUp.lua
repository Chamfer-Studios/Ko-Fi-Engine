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
		child:Active(false)
		-- print("why is it not working wtf")
		startTime = os.time()
		endTime = startTime+1
		stop = true
	end
	if (stop == true) then
		if (os.time() >= endTime) then
		--print("why is it not working wtf")
			SetVariable(false, name, "popUp", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL)
			stop = false
		end
	end
end

print("UI_ClosePopUp.lua compiled succesfully")