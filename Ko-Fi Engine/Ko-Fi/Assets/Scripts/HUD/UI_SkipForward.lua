-- Called each loop iteration
function Update(dt)
	if (gameObject.active == true) then
		if (gameObject:GetButton():IsPressed() == true) then
			if (GetVariable("UI_Skip.lua", "currentCredits", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) == 1) then
                SetVariable(2, "UI_Skip.lua", "currentCredits", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
            elseif (GetVariable("UI_Skip.lua", "currentCredits", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) == 2) then
                SetVariable(3, "UI_Skip.lua", "currentCredits", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
            elseif (GetVariable("UI_Skip.lua", "currentCredits", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) == 3) then
                SetVariable(4, "UI_Skip.lua", "currentCredits", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
            elseif (GetVariable("UI_Skip.lua", "currentCredits", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) == 4) then
                SetVariable(5, "UI_Skip.lua", "currentCredits", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
            end
		end
    end
end

print("UI_SkipForward.lua compiled succesfully")