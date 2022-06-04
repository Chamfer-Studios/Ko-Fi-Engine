isStarting = true
stop = false
name = "UI_DestroyPopUp.lua"

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
			DeleteGameObjectByUID(Find("UnlockPrefab"):GetUID())
		end
	end
end

print("UI_ClosePopUp.lua compiled succesfully")