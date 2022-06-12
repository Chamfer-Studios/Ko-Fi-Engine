isStarting = true
name = "Level_1"

local nameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
nameIV = InspectorVariable.new("name", nameIVT, name)
NewVariable(nameIV)

-- Called each loop iteration
function Update(dt)
    if (isStarting == true) then
        startTime = os.time()
        endTime = startTime+2
        isStarting = false
    end
    if os.time() >= endTime then
        gameObject:ChangeScene(true, name)
    end
end
    
print("UI_LoadingLvl1.lua compiled succesfully")