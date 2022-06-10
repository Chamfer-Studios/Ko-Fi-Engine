
-- Called each loop iteration
function UpdateUI(dt)
	if (gameObject.active == true) then
        if (gameObject:GetButton():IsPressed() == true) then
            DispatchGlobalEvent("Last_Checkpoint", {true})
        end
    end
end

print("UI_LastCheckpoint.lua compiled succesfully")