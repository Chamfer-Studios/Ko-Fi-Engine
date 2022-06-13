function Start()
    settings = Find("Settings Menu")
    pause = Find("Pause Menu")
end

-- Called each loop iteration
function UpdateUI(dt)
    if (gameObject.active == true) then
        if (gameObject:GetButton():IsPressed()) then
            settings:Active(false)
            pause:Active(false)
            ToggleRuntime()
        end
    end
end

print("UI_CloseSettings.lua compiled succesfully")
