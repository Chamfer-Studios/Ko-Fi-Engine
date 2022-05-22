id = -1
local idIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
idIV = InspectorVariable.new("id", idIVT, id)
NewVariable(idIV)

triggered = false


function OnTriggerEnter(go)
	
	if (go.tag == Tag.PLAYER and triggered == false) then
		DispatchGlobalEvent("DialogueTriggered", { id })
		triggered = true
	end
end
