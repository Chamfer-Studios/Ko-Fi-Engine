id = -1
local idIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
idIV = InspectorVariable.new("id", idIVT, id)
NewVariable(idIV)

triggered = false

function Start()
    if (not triggered and GetVariable("GameState.lua", "triggerDialogues", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) ==
        true) then
        DispatchGlobalEvent("DialogueTriggered", {id, nil, gameObject:GetUID()})
        DispatchGlobalEvent("DialoguePassed", {gameObject:GetUID()})
    end
end
