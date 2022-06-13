function Start()
    pause = Find("Pause Menu")
    settings = Find("Settings Menu")
end

-- Called each loop iteration
function UpdateUI(dt)
    if (gameObject.active == true) then
        if (gameObject:GetButton():IsPressed() and settings.active == false) then
            ToggleRuntime()
            pause:Active(false)
        end
    end
end

print("UI_ClosePause.lua compiled succesfully")
