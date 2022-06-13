id = -1
local idIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
idIV = InspectorVariable.new("id", idIVT, id)
NewVariable(idIV)

onlyNerala = false
local onlyNeralaIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL
onlyNeralaIV = InspectorVariable.new("onlyNerala", onlyNeralaIVT, onlyNerala)
NewVariable(onlyNeralaIV)

onlyOmozra = false
local onlyOmozraIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL
onlyOmozraIV = InspectorVariable.new("onlyOmozra", onlyOmozraIVT, onlyOmozra)
NewVariable(onlyOmozraIV)

unlockingNerala = false
local unlockingNeralaIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL
unlockingNeralaIV = InspectorVariable.new("unlockingNerala", unlockingNeralaIVT, unlockingNerala)
NewVariable(unlockingNeralaIV)

unlockingOmozra = false
local unlockingOmozraIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL
unlockingOmozraIV = InspectorVariable.new("unlockingOmozra", unlockingOmozraIVT, unlockingOmozra)
NewVariable(unlockingOmozraIV)

triggered = false

function Start()
    -- gameobjUID = gameObject:GetUID()
end

function OnTriggerEnter(go)
    -- Only nerala
    if onlyNerala == true then
        if (triggered == false and go:GetName() == "Nerala") then
            triggered = true
            if unlockingNerala == true then
                DispatchGlobalEvent("DialogueTriggered", {id, 2})
                DispatchGlobalEvent("DialoguePassed", {gameObject:GetUID()})
            elseif unlockingOmozra == true then
                DispatchGlobalEvent("DialogueTriggered", {id, 3})
                DispatchGlobalEvent("DialoguePassed", {gameObject:GetUID()})
            else
                if GetVariable("GameState.lua", "triggerDialogues", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true then
                    DispatchGlobalEvent("DialogueTriggered", {id, nil})
                    DispatchGlobalEvent("DialoguePassed", {gameObject:GetUID()})
                end
            end
        end
        -- Only omozra
    elseif onlyOmozra == true then
        if (triggered == false and go:GetName() == "Omozra") then
            triggered = true
            if unlockingNerala == true then
                DispatchGlobalEvent("DialogueTriggered", {id, 2})
                DispatchGlobalEvent("DialoguePassed", {gameObject:GetUID()})
            elseif unlockingOmozra == true then
                DispatchGlobalEvent("DialogueTriggered", {id, 3})
                DispatchGlobalEvent("DialoguePassed", {gameObject:GetUID()})
            else
                if GetVariable("GameState.lua", "triggerDialogues", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true then
                    DispatchGlobalEvent("DialogueTriggered", {id, nil})
                    DispatchGlobalEvent("DialoguePassed", {gameObject:GetUID()})
                end
            end

        end
        -- All
    else
        if (triggered == false and (go:GetName() == "Omozra" or go:GetName() == "Nerala" or go:GetName() == "Zhib")) then
            triggered = true
            if unlockingNerala == true then
                DispatchGlobalEvent("DialogueTriggered", {id, 2})
                DispatchGlobalEvent("DialoguePassed", {gameObject:GetUID()})
            elseif unlockingOmozra == true then
                DispatchGlobalEvent("DialogueTriggered", {id, 3})
                DispatchGlobalEvent("DialoguePassed", {gameObject:GetUID()})
            else
                if GetVariable("GameState.lua", "triggerDialogues", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true then
                    DispatchGlobalEvent("DialogueTriggered", {id, nil})
                    DispatchGlobalEvent("DialoguePassed", {gameObject:GetUID()})
                end
            end
        end
    end

end
