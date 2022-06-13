sceneName = "default_level"

local sceneNameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
sceneNameIV = InspectorVariable.new("sceneName", sceneNameIVT, sceneName)
NewVariable(sceneNameIV)

function EventHandler(key, fields)
    if (key == "FinalTrigger_Available") then
        gameObject:ChangeScene(true, sceneName)
    end
end

print("FinalTrigger.lua compiled succesfully")