function Start()
    pause = Find("Pause Menu")
    settings = Find("Settings Menu")
    pause:Active(false)
end

function UpdateUI(dt)
    if GetInput(25) == KEY_STATE.KEY_DOWN then
        if settings.active == true then
            settings:Active(false)
            pause:Active(true)
        else
            if pause.active == false then
                pause:Active(true)
                ToggleRuntime()
            else
                pause:Active(false)
                ToggleRuntime()
            end
        end
    end
end
