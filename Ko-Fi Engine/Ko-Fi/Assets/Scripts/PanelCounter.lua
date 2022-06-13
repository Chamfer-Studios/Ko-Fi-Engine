sceneName = "default_level"

local sceneNameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
sceneNameIV = InspectorVariable.new("sceneName", sceneNameIVT, sceneName)
NewVariable(sceneNameIV)

activatedPanels = 0

function Update(dt)
    if (activatedPanels == 3) then
		gameObject:ChangeScene(true, sceneName)
	end
end

function EventHandler(key, fields)
    if (key == "Panel_Activated") then
        activatedPanels = activatedPanels + 1
    end
end

print("PanelCounter.lua compiled succesfully")