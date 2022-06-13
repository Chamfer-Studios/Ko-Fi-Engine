function Start()
	hasTriggered = false
end

------------------ Collisions --------------------
function OnTriggerEnter(go)
	if (go.tag == Tag.PLAYER and hasTriggered == false) then
		hasTriggered = true
		DispatchGlobalEvent("Panel_Activated", {})
	end
end
--------------------------------------------------

print("PanelTrigger.lua compiled succesfully")