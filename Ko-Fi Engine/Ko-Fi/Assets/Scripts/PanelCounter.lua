activatedPanels = 0
finalTriggerAvailable = false

function Update(dt)
    if (activatedPanels == 3 and canContinue == false) then
        finalTriggerAvailable = true
        DispatchGlobalEvent("FinalTrigger_Available", {})
	end
end

function EventHandler(key, fields)
    if (key == "Panel_Activated") then
        activatedPanels = activatedPanels + 1
    end
end

print("PanelCounter.lua compiled succesfully")