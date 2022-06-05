function Update(dt)
    currentCharacterId = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
    if gameObject:GetButton():IsPressed() == true then
        DispatchGlobalEvent("Active_Ultimate", {currentCharacterId})
    end
    if gameObject:GetButton():IsIdle() == false and gameObject:GetButton():IsPressed() == false then
        DispatchGlobalEvent("Display_Description", {3, currentCharacterId})
    end
end
