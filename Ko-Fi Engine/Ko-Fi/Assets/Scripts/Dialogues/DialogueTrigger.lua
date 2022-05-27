id = -1
local idIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
idIV = InspectorVariable.new("id", idIVT, id)
NewVariable(idIV)

triggered = false

function OnTriggerEnter(go)

    if (triggered == false and (go:GetName() == "Zhib" or go:GetName() == "Nerala" or go:GetName() == "Omozra")) then
        DispatchGlobalEvent("DialogueTriggered", {id})
        triggered = true
    end
end
