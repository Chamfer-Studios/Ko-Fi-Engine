id = -1
local idIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
idIV = InspectorVariable.new("id", idIVT, id)
NewVariable(idIV)

triggered = false

function Start()
    if (not triggered) then
        DispatchGlobalEvent("DialogueTriggered", {id})
    end
end
