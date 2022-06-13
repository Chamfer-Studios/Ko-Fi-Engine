sceneName = "default_level"

local sceneNameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
sceneNameIV = InspectorVariable.new("sceneName", sceneNameIVT, sceneName)
NewVariable(sceneNameIV)

------------------ Collisions --------------------
function OnTriggerEnter(go)
	if (go.tag == Tag.PLAYER) then
		gameObject:ChangeScene(true, sceneName);
	end
end
--------------------------------------------------

print("WinTrigger.lua compiled succesfully")