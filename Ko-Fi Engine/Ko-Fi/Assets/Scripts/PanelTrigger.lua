hasTriggered = false

function Start()
	particleGO = gameObject:GetChildren()[1]
end

------------------ Collisions --------------------
function OnTriggerEnter(go)
	if (go.tag == Tag.PLAYER and hasTriggered == false) then
		hasTriggered = true
		DispatchGlobalEvent("Panel_Activated", {})
		particleGO:GetComponentParticle():StopParticleSpawn()
	end
end
--------------------------------------------------

print("PanelTrigger.lua compiled succesfully")