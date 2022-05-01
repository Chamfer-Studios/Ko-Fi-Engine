-- player = Find("Character")
name = "Level_1"
isStarting = true

local nameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
nameIV = InspectorVariable.new("name", nameIVT, name)
NewVariable(nameIV)

-- Called each loop iteration
function Update(dt)
	if (isStarting == true) then
		gameObject:GetParent():GetChild("LoadingScreen"):Active(false)
		isStarting = false
	end
	if (gameObject:GetButton():IsPressed() == true) then
		gameObject:GetParent():GetChild("LoadingScreen"):Active(true)
		startTime = os.time()
		endTime = startTime+1
	end
	if os.time() >= endTime then
		gameObject:ChangeScene(true, name)
	end
end

print("UI_ChangeScenes.lua compiled succesfully")