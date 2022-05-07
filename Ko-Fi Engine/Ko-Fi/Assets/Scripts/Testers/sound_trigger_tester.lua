constantSoundPos = nil

function Update(dt)
    if (GetInput(1) == KEY_STATE.KEY_DOWN) then
        GetGameObjectHovered()
        position = GetLastMouseClick()
        constantSoundPos = nil
        position = float3.new(position.x, position.y, position.z)
        DispatchGlobalEvent("Auditory_Trigger", { position, 20, "single", gameObject })
    end

    if (GetInput(3) == KEY_STATE.KEY_DOWN) then
        GetGameObjectHovered()
        position = GetLastMouseClick()
        constantSoundPos = float3.new(position.x, position.y, position.z)
    end

    if constantSoundPos ~= nil then
        DispatchGlobalEvent("Auditory_Trigger", { constantSoundPos, 20, "repeated", gameObject })
    end
end