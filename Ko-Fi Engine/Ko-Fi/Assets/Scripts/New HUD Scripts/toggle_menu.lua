isOpened = false
function Start()
    gameObject:Active(isOpened)
end
function UpdateUI(dt)
    if GetInput(25) == KEY_STATE.KEY_DOWN then
        isOpened = not isOpened
        gameObject:Active(isOpened)
        ToggleRuntime()
    end
end