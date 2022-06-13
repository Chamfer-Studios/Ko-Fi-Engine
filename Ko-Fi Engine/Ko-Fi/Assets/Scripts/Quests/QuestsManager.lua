function Start()

    questHolders = {gameObject:GetChild("Quest 1"), gameObject:GetChild("Quest 2"), gameObject:GetChild("Quest 3")}

    if (#questHolders == 3) then

        if (GetVariable("GameState.lua", "neralaAvailable", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == false and
            questHolders[1] ~= nil) then
            questHolders[1]:GetText():SetTextValue("Find Nerala")
        else
            questHolders[1]:GetText():SetTextValue(" ")
        end

        if (GetVariable("GameState.lua", "omozraAvailable", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == false and
            questHolders[2] ~= nil) then
            questHolders[2]:GetText():SetTextValue("Meet Omozra")
        else
            questHolders[2]:GetText():SetTextValue(" ")
        end

        if (questHolders[3] ~= nil) then
            questHolders[3]:GetText():SetTextValue("Kill Rabban")
        else
            questHolders[3]:GetText():SetTextValue(" ")
        end

    elseif (#questHolders == 4) then
        questHolders[1]:GetText():SetTextValue("Find and turn")
        questHolders[2]:GetText():SetTextValue("off the terminals")
        questHolders[3]:GetText():SetTextValue("Go to the")
        questHolders[4]:GetText():SetTextValue("throne room")
    end

end

function EventHandler(key, fields)
    if (key == "Enable_Character") then
        if (fields[1] == 2) then
            questHolders[1]:GetText():SetTextValue(" ")
        elseif (fields[1] == 3) then
            questHolders[2]:GetText():SetTextValue(" ")
        end
    elseif (key == "FinalTrigger_Available") then
        questHolders[1]:GetText():SetTextValue(" ")
    end
end

