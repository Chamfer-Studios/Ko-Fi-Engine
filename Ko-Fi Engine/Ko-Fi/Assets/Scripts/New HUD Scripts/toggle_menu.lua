isOpened = false
function Start()
    child = Find("Background")
    child:Active(isOpened)
end
function UpdateUI(dt)
    if (gameObject.active == true) then
        if GetInput(25) == KEY_STATE.KEY_DOWN then
            isOpened = not isOpened
            child:Active(isOpened)
            ToggleRuntime()
        end
    end
end