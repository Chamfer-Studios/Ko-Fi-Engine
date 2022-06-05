isOpened = false
function Update(dt)
    if GetInput(25) == KEY_STATE.KEY_DOWN then
        isOpened = not isOpened
        if isOpened == true then
            gameObject:Active(true)
        else
            gameObject:Active(false)
        end
    end
end