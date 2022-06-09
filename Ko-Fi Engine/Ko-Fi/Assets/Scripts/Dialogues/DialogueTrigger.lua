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

function OnTriggerEnter(go)

    -- Only nerala
    if onlyNerala == true then
        if (triggered == false and go:GetName() == "Nerala") then
            Log("Hol\n")
            DispatchGlobalEvent("DialogueTriggered", {id})
            triggered = true
            -- if unlockingNerala == true then
            --     DispatchGlobalEvent("CharacterUnlocked", {"Nerala"})
            -- elseif unlockingOmozra == true then
            --     DispatchGlobalEvent("CharacterUnlocked", {"Omozra"})
            -- end
        end
        -- Only omozra
    elseif onlyOmozra == true then
        if (triggered == false and go:GetName() == "Omozra") then
            DispatchGlobalEvent("DialogueTriggered", {id})
            triggered = true
            -- if unlockingNerala == true then
            --     DispatchGlobalEvent("CharacterUnlocked", {"Nerala"})
            -- elseif unlockingOmozra == true then
            --     DispatchGlobalEvent("CharacterUnlocked", {"Omozra"})
            -- end
        end
        -- All
    else
        if (triggered == false and (go:GetName() == "Omozra" or go:GetName() == "Nerala" or go:GetName() == "Zhib")) then
            DispatchGlobalEvent("DialogueTriggered", {id})
            triggered = true
            -- if unlockingNerala == true then
            --     DispatchGlobalEvent("CharacterUnlocked", {"Nerala"})
            -- elseif unlockingOmozra == true then
            --     DispatchGlobalEvent("CharacterUnlocked", {"Omozra"})
            -- end
        end
    end
end
