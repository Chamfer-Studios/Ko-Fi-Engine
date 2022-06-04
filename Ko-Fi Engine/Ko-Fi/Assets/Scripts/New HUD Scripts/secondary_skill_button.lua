function Update(dt)
    currentCharacterId = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
    if gameObject:GetButton():IsPressed() == true then
        DispatchGlobalEvent("Active_Secondary", {currentCharacterId})
    end
    if gameObject:GetButton():IsIdle() == false and gameObject:GetButton():IsPressed() == false then
        DispatchGlobalEvent("Display_Description", {2, currentCharacterId})
    end
end
