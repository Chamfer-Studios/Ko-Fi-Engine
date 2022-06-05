
local id = -1
local src = " "
local description = " "
local isStarting = true

local changedCharacter = false

local upgradeButton1Z = { state = false, object = Find("Skill 1 - Upgrade 1"), unlocked = true }
local upgradeButton2Z = { state = false, object = Find("Skill 1 - Upgrade 2"), unlocked = false }
local upgradeButton3Z = { state = false, object = Find("Skill 1 - Upgrade 3"), unlocked = false }
local upgradeButton4Z = { state = false, object = Find("Skill 2 - Upgrade 1"), unlocked = true }
local upgradeButton5Z = { state = false, object = Find("Skill 2 - Upgrade 2"), unlocked = false }
local upgradeButton6Z = { state = false, object = Find("Skill 2 - Upgrade 3"), unlocked = false }
local upgradeButton7Z = { state = false, object = Find("Skill 3 - Upgrade 1"), unlocked = true }
local upgradeButton8Z = { state = false, object = Find("Skill 3 - Upgrade 2"), unlocked = false }
local upgradeButton9Z = { state = false, object = Find("Skill 3 - Upgrade 3"), unlocked = false }
local upgradeButton10Z = { state = false, object = Find("Passive - Upgrade 1"), unlocked = true }
local upgradeButton11Z = { state = false, object = Find("Passive - Upgrade 2"), unlocked = false }
local upgradeButton12Z = { state = false, object = Find("Passive - Upgrade 3"), unlocked = false }

local upgradeButton1N = { state = false, object = Find("Skill 1 - Upgrade 1"), unlocked = true }
local upgradeButton2N = { state = false, object = Find("Skill 1 - Upgrade 2"), unlocked = false }
local upgradeButton3N = { state = false, object = Find("Skill 1 - Upgrade 3"), unlocked = false }
local upgradeButton4N = { state = false, object = Find("Skill 2 - Upgrade 1"), unlocked = true }
local upgradeButton5N = { state = false, object = Find("Skill 2 - Upgrade 2"), unlocked = false }
local upgradeButton6N = { state = false, object = Find("Skill 2 - Upgrade 3"), unlocked = false }
local upgradeButton7N = { state = false, object = Find("Skill 3 - Upgrade 1"), unlocked = true }
local upgradeButton8N = { state = false, object = Find("Skill 3 - Upgrade 2"), unlocked = false }
local upgradeButton9N = { state = false, object = Find("Skill 3 - Upgrade 3"), unlocked = false }
local upgradeButton10N = { state = false, object = Find("Passive - Upgrade 1"), unlocked = true }
local upgradeButton11N = { state = false, object = Find("Passive - Upgrade 2"), unlocked = false }
local upgradeButton12N = { state = false, object = Find("Passive - Upgrade 3"), unlocked = false }

local upgradeButton1O = { state = false, object = Find("Skill 1 - Upgrade 1"), unlocked = true }
local upgradeButton2O = { state = false, object = Find("Skill 1 - Upgrade 2"), unlocked = false }
local upgradeButton3O = { state = false, object = Find("Skill 1 - Upgrade 3"), unlocked = false }
local upgradeButton4O = { state = false, object = Find("Skill 2 - Upgrade 1"), unlocked = true }
local upgradeButton5O = { state = false, object = Find("Skill 2 - Upgrade 2"), unlocked = false }
local upgradeButton6O = { state = false, object = Find("Skill 2 - Upgrade 3"), unlocked = false }
local upgradeButton7O = { state = false, object = Find("Skill 3 - Upgrade 1"), unlocked = true }
local upgradeButton8O = { state = false, object = Find("Skill 3 - Upgrade 2"), unlocked = false }
local upgradeButton9O = { state = false, object = Find("Skill 3 - Upgrade 3"), unlocked = false }
local upgradeButton10O = { state = false, object = Find("Passive - Upgrade 1"), unlocked = true }
local upgradeButton11O = { state = false, object = Find("Passive - Upgrade 2"), unlocked = false }
local upgradeButton12O = { state = false, object = Find("Passive - Upgrade 3"), unlocked = false }

local upgradeArray = {upgradeButton1Z, upgradeButton2Z, upgradeButton3Z, upgradeButton4Z, 
upgradeButton5Z, upgradeButton6Z, upgradeButton7Z, upgradeButton8Z, upgradeButton9Z, 
upgradeButton10Z, upgradeButton11Z, upgradeButton12Z}
local path = "Assets/Scenes/SceneTransitionUI/sceneTransition.json"

function calcAbilityCost(level, ultimate)
    if(ultimate ~= nil) then
        local sum = level + 1
        return 2^sum * 100
    end

    local sum = level + 1
    return 2^sum * 150
end

------------ Dialogue Manager ------------
function Start()
    LoadGameState()

    localSpice = GetGameJsonInt("spice")
    SpiceAmountText = Find("SpiceAmount")
    Log("Spice Amount: " .. localSpice .. "\n")
    --Log(upgradeButton.object:GetName())
    LoadJsonFile(path)
    skillUiArray = {1,2,3, 101}
    dialogueGo = Find("Dialogue")
    id = 0
    UpdateUI()
end

function Update(dt)
    if(isStarting == true) then
        SpiceCost(0)
        isStarting = false
    end
    if(Find("Button") == nil) then
        Log("No button")
    end
    
    if(Find("Button"):GetButton():IsIdle() == false and Find("Button"):GetButton():IsPressed() == false) then
        SetDialogValue(1)
    elseif(Find("Button (1)"):GetButton():IsIdle() == false and Find("Button (1)"):GetButton():IsPressed() == false) then
        SetDialogValue(2)
    elseif(Find("Button (2)"):GetButton():IsIdle() == false and Find("Button (2)"):GetButton():IsPressed() == false) then
        SetDialogValue(3)
    elseif(Find("Button (3)"):GetButton():IsIdle() == false and Find("Button (3)"):GetButton():IsPressed() == false) then
        SetDialogValue(4)
    else 
        SetDialogValue(0)
    end



    --for narnia
    if(upgradeArray[1].object:GetButton():IsPressed())then
    --FIRST HAB
    -----------------------------------------------------
    if(upgradeArray[1].state == false) then
        if(id == 0)then
            LevelUpZhibSkill(1, 1, true)
        elseif(id == 1) then
            LevelUpNeralaSkill(1, 1, true)
        elseif(id == 2) then
            LevelUpOmozraSkill(1, 1, true)
        end
        SpiceCost(20)
        upgradeArray[1].state = true
        upgradeArray[1].object:GetChildren()[1]:Active(true)
        --Unlock
        upgradeArray[2].unlocked = true
    else
        if upgradeArray[2].state == false then
            upgradeArray[1].state = false
            upgradeArray[1].object:GetChildren()[1]:Active(false)
            upgradeArray[2].unlocked = false
            if(id == 0)then
                LevelUpZhibSkill(1, 1, false)
            elseif(id == 1) then
                LevelUpNeralaSkill(1, 1, false)
            elseif(id == 2) then
                LevelUpOmozraSkill(1, 1, false)
            end
            SpiceCost(-20)
        end
        end
    elseif(upgradeArray[2].object:GetButton():IsPressed()) then
        if(upgradeArray[2].state == false and upgradeArray[2].unlocked == true) then
            if(id == 0)then
                LevelUpZhibSkill(2, 1, true)
            elseif(id == 1) then
                LevelUpNeralaSkill(2, 1, true)
            elseif(id == 2) then
                LevelUpOmozraSkill(2, 1, true)
            end
            SpiceCost(20)
            upgradeArray[2].state = true
            upgradeArray[2].object:GetChildren()[1]:Active(true)
            --Unlock
            upgradeArray[3].unlocked = true
    else
        if upgradeArray[3].state == false then
        upgradeArray[2].state = false
        upgradeArray[2].object:GetChildren()[1]:Active(false)
        upgradeArray[3].unlocked = false
        if(id == 0)then
            LevelUpZhibSkill(2, 1, false)
        elseif(id == 1) then
            LevelUpNeralaSkill(2, 1, false)
        elseif(id == 2) then
            LevelUpOmozraSkill(2, 1, false)
        end
        SpiceCost(-20)
        end
    end
    elseif(upgradeArray[3].object:GetButton():IsPressed()) then
        if(upgradeArray[3].state == false and upgradeArray[3].unlocked == true) then
            if(id == 0)then
                LevelUpZhibSkill(3, 1, true)
            elseif(id == 1) then
                LevelUpNeralaSkill(3, 1, true)
            elseif(id == 2) then
                LevelUpOmozraSkill(3, 1, true)
            end
            SpiceCost(20)
            upgradeArray[3].state = true
            upgradeArray[3].object:GetChildren()[1]:Active(true)
        else
            upgradeArray[3].state = false
            upgradeArray[3].object:GetChildren()[1]:Active(false)
            if(id == 0)then
                LevelUpZhibSkill(3, 1,false)
            elseif(id == 1) then
                LevelUpNeralaSkill(3, 1,false)
            elseif(id == 2) then
                LevelUpOmozraSkill(3, 1,false)
            end
            SpiceCost(-20)
        end
    --SECOND HAB
    -----------------------------------------------------
    elseif(upgradeArray[4].object:GetButton():IsPressed()) then
        if(upgradeArray[4].state == false) then
            if(id == 0)then
                LevelUpZhibSkill(1,2, true)
            elseif(id == 1) then
                LevelUpNeralaSkill(1,2, true)
            elseif(id == 2) then
                LevelUpOmozraSkill(1,2, true)
            end
            SpiceCost(20)
            upgradeArray[4].state = true
            upgradeArray[4].object:GetChildren()[1]:Active(true)
            --Unlock
            upgradeArray[5].unlocked = true
        else
            if upgradeArray[5].state == false then
            upgradeArray[4].state = false
            upgradeArray[4].object:GetChildren()[1]:Active(false)
            upgradeArray[5].unlocked = false
            if(id == 0)then
                LevelUpZhibSkill(1,2, false)
            elseif(id == 1) then
                LevelUpNeralaSkill(1,2, false)
            elseif(id == 2) then
                LevelUpOmozraSkill(1,2, false)
            end
            SpiceCost(-20)
            end
        end
    elseif(upgradeArray[5].object:GetButton():IsPressed()) then
        if(upgradeArray[5].state == false and upgradeArray[5].unlocked == true) then
            if(id == 0)then
                LevelUpZhibSkill(2,2, true)
            elseif(id == 1) then
                LevelUpNeralaSkill(2,2, true)
            elseif(id == 2) then
                LevelUpOmozraSkill(2,2, true)
            end
            SpiceCost(20)
            upgradeArray[5].state = true
            upgradeArray[5].object:GetChildren()[1]:Active(true)
            --Unlock
            upgradeArray[6].unlocked = true
        else
            if upgradeArray[6].state == false then
            upgradeArray[5].state = false
            upgradeArray[5].object:GetChildren()[1]:Active(false)
            upgradeArray[6].unlocked = false
            if(id == 0)then
                LevelUpZhibSkill(2,2, false)
            elseif(id == 1) then
                LevelUpNeralaSkill(2,2, false)
            elseif(id == 2) then
                LevelUpOmozraSkill(2,2, false)
            end
            SpiceCost(-20)
            end
        end
    elseif(upgradeArray[6].object:GetButton():IsPressed()) then
        if(upgradeArray[6].state == false and upgradeArray[6].unlocked == true) then
            if(id == 0)then
                LevelUpZhibSkill(3,2, true)
            elseif(id == 1) then
                LevelUpNeralaSkill(3,2, true)
            elseif(id == 2) then
                LevelUpOmozraSkill(3,2, true)
            end
            SpiceCost(20)
            upgradeArray[6].state = true
            upgradeArray[6].object:GetChildren()[1]:Active(true)
        else
            upgradeArray[6].state = false
            upgradeArray[6].object:GetChildren()[1]:Active(false)
            if(id == 0)then
                LevelUpZhibSkill(3,2, false)
            elseif(id == 1) then
                LevelUpNeralaSkill(3,2, false)
            elseif(id == 2) then
                LevelUpOmozraSkill(3,2, false)
            end
            SpiceCost(-20)
        end
    --THIRD HAB
    -----------------------------------------------------
    elseif(upgradeArray[7].object:GetButton():IsPressed()) then
        if(upgradeArray[7].state == false) then
            if(id == 0)then
                LevelUpZhibSkill(1,3, true)
            elseif(id == 1) then
                LevelUpNeralaSkill(1,3, true)
            elseif(id == 2) then
                LevelUpOmozraSkill(1,3, true)
            end
            
            SpiceCost(20)
            upgradeArray[7].state = true
            upgradeArray[7].object:GetChildren()[1]:Active(true)
            --Unlock
            upgradeArray[8].unlocked = true
        else
            if upgradeArray[8].state == false then
                upgradeArray[7].state = false
                upgradeArray[7].object:GetChildren()[1]:Active(false)
                upgradeArray[8].unlocked = false
                if(id == 0)then
                    LevelUpZhibSkill(1,3, false)
                elseif(id == 1) then
                    LevelUpNeralaSkill(1,3, false)
                elseif(id == 2) then
                    LevelUpOmozraSkill(1,3, false)
                end
                SpiceCost(-20)
            end
        end
    elseif(upgradeArray[8].object:GetButton():IsPressed()) then
        if(upgradeArray[8].state == false and upgradeArray[8].unlocked == true) then
            if(id == 0)then
                LevelUpZhibSkill(2,3, true)
            elseif(id == 1) then
                LevelUpNeralaSkill(2,3, true)
            elseif(id == 2) then
                LevelUpOmozraSkill(2,3, true)
            end
            SpiceCost(20)
            upgradeArray[8].state = true
            upgradeArray[8].object:GetChildren()[1]:Active(true)
            --Unlock
            upgradeArray[9].unlocked = true
        else
            if upgradeArray[9].state == false then
            upgradeArray[8].state = false
            upgradeArray[8].object:GetChildren()[1]:Active(false)
            upgradeArray[9].unlocked = false
            if(id == 0)then
                LevelUpZhibSkill(2,3, false)
            elseif(id == 1) then
                LevelUpNeralaSkill(2,3, false)
            elseif(id == 2) then
                LevelUpOmozraSkill(2,3, false)
            end
            SpiceCost(-20)
            end
        end
    elseif(upgradeArray[9].object:GetButton():IsPressed()) then
        if(upgradeArray[9].state == false and upgradeArray[9].unlocked == true) then
            if(id == 0)then
                LevelUpZhibSkill(3,3, true)
            elseif(id == 1) then
                LevelUpNeralaSkill(3,3, true)
            elseif(id == 2) then
                LevelUpOmozraSkill(3,3, true)
            end
            SpiceCost(20)
            upgradeArray[9].state = true
            upgradeArray[9].object:GetChildren()[1]:Active(true)
        else
            upgradeArray[9].state = false
            upgradeArray[9].object:GetChildren()[1]:Active(false)
            if(id == 0)then
                LevelUpZhibSkill(3,3, false)
            elseif(id == 1) then
                LevelUpNeralaSkill(3,3, false)
            elseif(id == 2) then
                LevelUpOmozraSkill(3,3, false)
            end
            SpiceCost(-20)
        end
    --PASIVE
    -----------------------------------------------------
    elseif(upgradeArray[10].object:GetButton():IsPressed()) then
        if(upgradeArray[10].state == false) then
            if(id == 0)then
                LevelUpZhibSkill(1,4, true)
            elseif(id == 1) then
                LevelUpNeralaSkill(1,4, true)
            elseif(id == 2) then
                LevelUpOmozraSkill(1,4, true)
            end
            SpiceCost(20)
            upgradeArray[10].state = true
            upgradeArray[10].object:GetChildren()[1]:Active(true)
            --Unlock
            upgradeArray[11].unlocked = true
        else
            if upgradeArray[11].state == false then
            upgradeArray[10].state = false
            upgradeArray[10].object:GetChildren()[1]:Active(false)
            upgradeArray[11].unlocked = false
            if(id == 0)then
                LevelUpZhibSkill(1,4, false)
            elseif(id == 1) then
                LevelUpNeralaSkill(1,4, false)
            elseif(id == 2) then
                LevelUpOmozraSkill(1,4, false)
            end
            SpiceCost(-20)
            end
        end
    elseif(upgradeArray[11].object:GetButton():IsPressed()) then
        if(upgradeArray[11].state == false and upgradeArray[11].unlocked == true) then
            if(id == 0)then
                LevelUpZhibSkill(2,4, true)
            elseif(id == 1) then
                LevelUpNeralaSkill(2,4, true)
            elseif(id == 2) then
                LevelUpOmozraSkill(2,4, true)
            end
            SpiceCost(20)
            upgradeArray[11].state = true
            upgradeArray[11].object:GetChildren()[1]:Active(true)
            --Unlock
            upgradeArray[12].unlocked = true
        else
            if upgradeArray[12].state == false then
            upgradeArray[11].state = false
            upgradeArray[11].object:GetChildren()[1]:Active(false)
            upgradeArray[12].unlocked = false
            if(id == 0)then
                LevelUpZhibSkill(2,4, false)
            elseif(id == 1) then
                LevelUpNeralaSkill(2,4, false)
            elseif(id == 2) then
                LevelUpOmozraSkill(2,4, false)
            end
            SpiceCost(-20)
            end
        end
    elseif(upgradeArray[12].object:GetButton():IsPressed()) then
        if(upgradeArray[12].state == false and upgradeArray[12].unlocked == true) then
            if(id == 0)then
                LevelUpZhibSkill(3,4, true)
            elseif(id == 1) then
                LevelUpNeralaSkill(3,4, true)
            elseif(id == 2) then
                LevelUpOmozraSkill(3,4, true)
            end
            SpiceCost(20)
            upgradeArray[12].state = true
            upgradeArray[12].object:GetChildren()[1]:Active(true)
        else
            upgradeArray[12].state = false
            upgradeArray[12].object:GetChildren()[1]:Active(false)
            if(id == 0)then
                LevelUpZhibSkill(3,4, false)
            elseif(id == 1) then
                LevelUpNeralaSkill(3,4, false)
            elseif(id == 2) then
                LevelUpOmozraSkill(3,4, false)
            end
            SpiceCost(-20)
        end
    end
    


end

function EventHandler(key, fields)
    if (key == "TransitionedFromLastCharacter") then -- fields[1] -> go;
        id = fields[1]
        changedCharacter = true
        UpdateUI()
    end
end
------------ END Dialogue Manager ------------

------------ Dialogue ------------

function UpdateUI()
    
    if(changedCharacter == true) then
        if(id == 0)then
            skillUiArray = {1,2,3, 101}

            upgradeArray = {upgradeButton1Z, upgradeButton2Z, upgradeButton3Z, upgradeButton4Z, 
            upgradeButton5Z, upgradeButton6Z, upgradeButton7Z, upgradeButton8Z, upgradeButton9Z, 
            upgradeButton10Z, upgradeButton11Z, upgradeButton12Z}
        elseif(id == 1) then
            skillUiArray = {4,5,6, 102}

            upgradeArray = {upgradeButton1N, upgradeButton2N, upgradeButton3N, upgradeButton4N, 
            upgradeButton5N, upgradeButton6N, upgradeButton7N, upgradeButton8N, upgradeButton9N, 
            upgradeButton10N, upgradeButton11N, upgradeButton12N}
        elseif(id == 2) then
            skillUiArray = {7,8,9, 103}

            upgradeArray = {upgradeButton1O, upgradeButton2O, upgradeButton3O, upgradeButton4O, 
            upgradeButton5O, upgradeButton6O, upgradeButton7O, upgradeButton8O, upgradeButton9O, 
            upgradeButton10O, upgradeButton11O, upgradeButton12O}
        end

        SetSkillsValue()
    end
end

function SetSkillsValue()
    -- Get Dialogue Values From JSON
    -- print("setting Values")

    src = GetTransString("src", skillUiArray[1])
    src2 = GetTransString("src", skillUiArray[2])
    src3 = GetTransString("src", skillUiArray[3])
    src4 = GetTransString("src", skillUiArray[4])
    
    -- Set Values To The Prefab
    Find("SkillOneUI"):GetImage():SetTexture(src)
    Find("feedbackOne"):GetImage():SetTexture(src)
    Find("SkillTwoUI"):GetImage():SetTexture(src2)
    Find("feedbackTwo"):GetImage():SetTexture(src2)
    Find("SkillThreeUI"):GetImage():SetTexture(src3)
    Find("feedbackThree"):GetImage():SetTexture(src3)
    Find("PassiveUI"):GetImage():SetTexture(src4)

    for i = 1, #upgradeArray do
        if upgradeArray[i].state == false then
            upgradeArray[i].object:GetChildren()[1]:Active(false)
        else
            upgradeArray[i].object:GetChildren()[1]:Active(true)
        end
    end

    changedCharacter = false;
end

function SetDialogValue(index)
    -- Get Dialogue Values From JSON
    -- print("setting Values")
    if(index == 1) then
        dialogueGo:Active(true)
        dialog = GetTransString("description1", skillUiArray[1])
        dialog1 = GetTransString("description2", skillUiArray[1])
        dialog2 = GetTransString("description3", skillUiArray[1])
        dialog3 = GetTransString("description4", skillUiArray[1])
        dialog4 = GetTransString("description5", skillUiArray[1])
        dialog5 = GetTransString("description6", skillUiArray[1])
    elseif(index == 2) then
        dialogueGo:Active(true)
        dialog = GetTransString("description1", skillUiArray[2])
        dialog1 = GetTransString("description2", skillUiArray[2])
        dialog2 = GetTransString("description3", skillUiArray[2])
        dialog3 = GetTransString("description4", skillUiArray[2])
        dialog4 = GetTransString("description5", skillUiArray[2])
        dialog5 = GetTransString("description6", skillUiArray[2])
    elseif(index == 3) then
        dialogueGo:Active(true)
        dialog = GetTransString("description1", skillUiArray[3])
        dialog1 = GetTransString("description2", skillUiArray[3])
        dialog2 = GetTransString("description3", skillUiArray[3])
        dialog3 = GetTransString("description4", skillUiArray[3])
        dialog4 = GetTransString("description5", skillUiArray[3])
        dialog5 = GetTransString("description6", skillUiArray[3])
    elseif(index == 4) then
        dialogueGo:Active(true)
        dialog = GetTransString("description1", skillUiArray[4])
        dialog1 = GetTransString("description2", skillUiArray[4])
        dialog2 = GetTransString("description3", skillUiArray[4])
        dialog3 = GetTransString("description4", skillUiArray[4])
        dialog4 = GetTransString("description5", skillUiArray[4])
        dialog5 = GetTransString("description6", skillUiArray[4])
    else
        dialogueGo:Active(false)
        dialog = " "
        dialog1 = " "
        dialog2 = " "
        dialog3 = " "
        dialog4 = " "
        dialog5 = " "
    end
    
     -- Set Values To The Dialogue
     Find("Text"):GetText():SetTextValue(dialog)
     Find("Text (1)"):GetText():SetTextValue(dialog1)
     Find("Text (2)"):GetText():SetTextValue(dialog2)
     Find("Text (3)"):GetText():SetTextValue(dialog3)
     Find("Text (4)"):GetText():SetTextValue(dialog4)
     Find("Text (5)"):GetText():SetTextValue(dialog5)
    

end

function SpiceCost(amount)
    newSpice = GetVariable("UI_GameState.lua", "spice", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) - amount
    SpiceAmountText:GetText():SetTextValue(newSpice)
    Log("New Spice:" .. newSpice .. "\n")
    SetVariable(newSpice, "UI_GameState.lua", "spice", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
    Log("Saved Spice:" .. GetVariable("UI_GameState.lua", "spice", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) .. "\n")
end

function LevelUpZhibSkill(skillLvl, skillId, direction)

    if(direction == false) then
       skillLvl = skillLvl - 1
    end
   
    if(skillId == 1) then
        SetVariable(skillLvl, "UI_GameState.lua", "zhib_primary_level", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        str = "Zhib first skill level" .. GetVariable("UI_GameState.lua", "zhib_primary_level", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) .. "\n"
        Log(str)
    elseif(skillId == 2) then
        SetVariable(skillLvl, "UI_GameState.lua", "zhib_secondary_level", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        str = "Zhib second skill level" .. GetVariable("UI_GameState.lua", "zhib_secondary_level", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) .. "\n"
        Log(str)
    elseif(skillId == 3) then
        SetVariable(skillLvl, "UI_GameState.lua", "zhib_ultimate_level", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        str = "Zhib third skill level" .. GetVariable("UI_GameState.lua", "zhib_ultimate_level", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) .. "\n"
        Log(str)
    elseif(skillId == 4) then
        --SetVariable(skillLvl, "UI_GameState.lua", "zhib_primary_level", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        --str = "Zhib level" .. GetVariable("UI_GameState.lua", "zhib_primary_level", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) .. "\n"
        --Log(str)
    end
    
    Log("LevelingUpZhib \n")
end

function LevelUpNeralaSkill(skillLvl, skillId, direction)

    if(direction == false) then
        skillLvl = skillLvl - 1
     end

    if(skillId == 1) then
        SetVariable(skillLvl, "UI_GameState.lua", "nerala_primary_level", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        str = "Nerala first skill level" .. GetVariable("UI_GameState.lua", "nerala_primary_level", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) .. "\n"
        Log(str)
    elseif(skillId == 2) then
        SetVariable(skillLvl, "UI_GameState.lua", "nerala_secondary_level", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        str = "Nerala secondary skill level" .. GetVariable("UI_GameState.lua", "nerala_secondary_level", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) .. "\n"
        Log(str)
    elseif(skillId == 3) then
        SetVariable(skillLvl, "UI_GameState.lua", "nerala_ultimate_level", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        str = "Nerala third skill level" .. GetVariable("UI_GameState.lua", "nerala_ultimate_level", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) .. "\n"
        Log(str)
    elseif(skillId == 4) then
        --levelUpNerala4 = skillLvl
    end
    Log("LevelingUpNerala")
end

function LevelUpOmozraSkill(skillLvl, skillId, direction) --false down true up

    if(direction == false) then
        skillLvl = skillLvl - 1
    end

    if(skillId == 1) then
        SetVariable(skillLvl, "UI_GameState.lua", "omozra_primary_level", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        str = "Omozra first skill level" .. GetVariable("UI_GameState.lua", "omozra_primary_level", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) .. "\n"
        Log(str)
    elseif(skillId == 2) then
        SetVariable(skillLvl, "UI_GameState.lua", "omozra_secondary_level", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        str = "Omozra second skill level" .. GetVariable("UI_GameState.lua", "omozra_secondary_level", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) .. "\n"
        Log(str)
    elseif(skillId == 3) then
        SetVariable(skillLvl, "UI_GameState.lua", "omozra_ultimate_level", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        str = "Omozra third skill level" .. GetVariable("UI_GameState.lua", "omozra_ultimate_level", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) .. "\n"
        Log(str)
    elseif(skillId == 4) then
        --levelUpNerala4 = skillLvl
    end
    Log("LevelingUpOmozra\n")
end

------------ END Dialogue ------------
print("Scene Transition Script Load Success")
