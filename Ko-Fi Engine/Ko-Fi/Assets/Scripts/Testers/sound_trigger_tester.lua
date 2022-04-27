constantSoundPos = nil

function Update(dt)
    if (GetInput(1) == KEY_STATE.KEY_DOWN) then
        GetGameObjectHovered()
        position = GetLastMouseClick()
        constantSoundPos = nil
        DispatchGlobalEvent("Auditory_Trigger", { position, 20, "single", nil })
    end

    if (GetInput(3) == KEY_STATE.KEY_DOWN) then
        GetGameObjectHovered()
        position = GetLastMouseClick()
        constantSoundPos = position
    end

    if constantSoundPos ~= nil then
        DispatchGlobalEvent("Auditory_Trigger", { constantSoundPos, 20, "repeated", nil })
    end
end