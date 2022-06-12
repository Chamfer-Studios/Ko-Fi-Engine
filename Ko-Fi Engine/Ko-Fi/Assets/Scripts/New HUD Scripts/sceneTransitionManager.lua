local upgradeButton1Z = {
    state = false,
    object = Find("Skill 1 - Upgrade 1"),
    unlocked = true
}
local upgradeButton2Z = {
    state = false,
    object = Find("Skill 1 - Upgrade 2"),
    unlocked = false
}
local upgradeButton3Z = {
    state = false,
    object = Find("Skill 1 - Upgrade 3"),
    unlocked = false
}
local upgradeButton4Z = {
    state = false,
    object = Find("Skill 2 - Upgrade 1"),
    unlocked = true
}
local upgradeButton5Z = {
    state = false,
    object = Find("Skill 2 - Upgrade 2"),
    unlocked = false
}
local upgradeButton6Z = {
    state = false,
    object = Find("Skill 2 - Upgrade 3"),
    unlocked = false
}
local upgradeButton7Z = {
    state = false,
    object = Find("Skill 3 - Upgrade 1"),
    unlocked = true
}
local upgradeButton8Z = {
    state = false,
    object = Find("Skill 3 - Upgrade 2"),
    unlocked = false
}
local upgradeButton9Z = {
    state = false,
    object = Find("Skill 3 - Upgrade 3"),
    unlocked = false
}
local upgradeButton10Z = {
    state = false,
    object = Find("Passive - Upgrade 1"),
    unlocked = true
}
local upgradeButton11Z = {
    state = false,
    object = Find("Passive - Upgrade 2"),
    unlocked = false
}
local upgradeButton12Z = {
    state = false,
    object = Find("Passive - Upgrade 3"),
    unlocked = false
}

local upgradeButton1N = {
    state = false,
    object = Find("Skill 1 - Upgrade 1"),
    unlocked = true
}
local upgradeButton2N = {
    state = false,
    object = Find("Skill 1 - Upgrade 2"),
    unlocked = false
}
local upgradeButton3N = {
    state = false,
    object = Find("Skill 1 - Upgrade 3"),
    unlocked = false
}
local upgradeButton4N = {
    state = false,
    object = Find("Skill 2 - Upgrade 1"),
    unlocked = true
}
local upgradeButton5N = {
    state = false,
    object = Find("Skill 2 - Upgrade 2"),
    unlocked = false
}
local upgradeButton6N = {
    state = false,
    object = Find("Skill 2 - Upgrade 3"),
    unlocked = false
}
local upgradeButton7N = {
    state = false,
    object = Find("Skill 3 - Upgrade 1"),
    unlocked = true
}
local upgradeButton8N = {
    state = false,
    object = Find("Skill 3 - Upgrade 2"),
    unlocked = false
}
local upgradeButton9N = {
    state = false,
    object = Find("Skill 3 - Upgrade 3"),
    unlocked = false
}
local upgradeButton10N = {
    state = false,
    object = Find("Passive - Upgrade 1"),
    unlocked = true
}
local upgradeButton11N = {
    state = false,
    object = Find("Passive - Upgrade 2"),
    unlocked = false
}
local upgradeButton12N = {
    state = false,
    object = Find("Passive - Upgrade 3"),
    unlocked = false
}

local upgradeButton1O = {
    state = false,
    object = Find("Skill 1 - Upgrade 1"),
    unlocked = true
}
local upgradeButton2O = {
    state = false,
    object = Find("Skill 1 - Upgrade 2"),
    unlocked = false
}
local upgradeButton3O = {
    state = false,
    object = Find("Skill 1 - Upgrade 3"),
    unlocked = false
}
local upgradeButton4O = {
    state = false,
    object = Find("Skill 2 - Upgrade 1"),
    unlocked = true
}
local upgradeButton5O = {
    state = false,
    object = Find("Skill 2 - Upgrade 2"),
    unlocked = false
}
local upgradeButton6O = {
    state = false,
    object = Find("Skill 2 - Upgrade 3"),
    unlocked = false
}
local upgradeButton7O = {
    state = false,
    object = Find("Skill 3 - Upgrade 1"),
    unlocked = true
}
local upgradeButton8O = {
    state = false,
    object = Find("Skill 3 - Upgrade 2"),
    unlocked = false
}
local upgradeButton9O = {
    state = false,
    object = Find("Skill 3 - Upgrade 3"),
    unlocked = false
}
local upgradeButton10O = {
    state = false,
    object = Find("Passive - Upgrade 1"),
    unlocked = true
}
local upgradeButton11O = {
    state = false,
    object = Find("Passive - Upgrade 2"),
    unlocked = false
}
local upgradeButton12O = {
    state = false,
    object = Find("Passive - Upgrade 3"),
    unlocked = false
}
local id = -1
local src = " "
local description = " "
local isStarting = true

local changedCharacter = false

local upgradeArray = {upgradeButton1Z, upgradeButton2Z, upgradeButton3Z, upgradeButton4Z, upgradeButton5Z,
                      upgradeButton6Z, upgradeButton7Z, upgradeButton8Z, upgradeButton9Z, upgradeButton10Z,
                      upgradeButton11Z, upgradeButton12Z}

local path = "Assets/Descriptions/SceneTransitionDescriptions.json"

local primaryLvl1Cost = 200
local ultimateLvl1Cost = 300

function calcAbilityCost(level, flags)
    if (flags == "ultimate") then
        return math.floor(2 ^ level * 150)
    elseif (flags == "passive") then
        return math.floor(2 ^ level * 50)
    end

    return math.floor(2 ^ level * 100)
end

------------ Dialogue Manager ------------
function Start()

    SpiceAmountText = Find("SpiceAmount")

    LoadJsonFile(path)

    skillUiArray = {1, 2, 3, 101}
    dialogueGo = Find("Dialogue")

    id = 0

end

function Update(dt)
    if (isStarting == true) then

        -- Deactivate the characters that are not available
        if (GetVariable("UI_GameState.lua", "omozra_available", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == false) then
            Find("RightArrow"):Active(false)
            Find("omozra_transition"):Active(false)
        end
        if (GetVariable("UI_GameState.lua", "nerala_available", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == false) then
            Find("LeftArrow"):Active(false)
            Find("nerala_transition"):Active(false)
        end

        UpdateUI()

        SpiceCost(-8500)

        isStarting = false
    end

    if (Find("Button") == nil) then
        Log("No button")
    end

    if (Find("Button"):GetButton():IsIdle() == false and Find("Button"):GetButton():IsPressed() == false) then
        SetDialogValue(1)
    elseif (Find("Button (1)"):GetButton():IsIdle() == false and Find("Button (1)"):GetButton():IsPressed() == false) then
        SetDialogValue(2)
    elseif (Find("Button (2)"):GetButton():IsIdle() == false and Find("Button (2)"):GetButton():IsPressed() == false) then
        SetDialogValue(3)
    elseif (Find("Button (3)"):GetButton():IsIdle() == false and Find("Button (3)"):GetButton():IsPressed() == false) then
        SetDialogValue(4)
    else
        SetDialogValue(0)
    end

    -- PRIMARY ABILITY : LEVEL 1
    if (upgradeArray[1].object:GetButton():IsPressed() and upgradeArray[1].unlocked == true) then
        if (upgradeArray[1].state == false) then

            if (LevelUp(id, "primary", 1, true) ~= false) then
                upgradeArray[1].state = true
                upgradeArray[1].object:GetChildren()[1]:Active(true)
                -- Unlock
                upgradeArray[2].unlocked = true
            end

        else
            if upgradeArray[2].state == false then
                upgradeArray[1].state = false
                upgradeArray[1].object:GetChildren()[1]:Active(false)
                upgradeArray[2].unlocked = false

                LevelUp(id, "primary", 1, false)
            end
        end
        -- PRIMARY ABILITY : LEVEL 2
    elseif (upgradeArray[2].object:GetButton():IsPressed() and upgradeArray[2].unlocked == true) then
        if (upgradeArray[2].state == false) then

            if (LevelUp(id, "primary", 2, true) ~= false) then
                upgradeArray[2].state = true
                upgradeArray[2].object:GetChildren()[1]:Active(true)
                -- Unlock
                upgradeArray[3].unlocked = true
            end
        else
            if upgradeArray[3].state == false then
                upgradeArray[2].state = false
                upgradeArray[2].object:GetChildren()[1]:Active(false)
                upgradeArray[3].unlocked = false

                LevelUp(id, "primary", 2, false)
            end
        end
        -- PRIMARY ABILITY : LEVEL 3
    elseif (upgradeArray[3].object:GetButton():IsPressed() and upgradeArray[3].unlocked == true) then
        if (upgradeArray[3].state == false) then

            if (LevelUp(id, "primary", 3, true) ~= false) then
                upgradeArray[3].state = true
                upgradeArray[3].object:GetChildren()[1]:Active(true)
            end
        else
            upgradeArray[3].state = false
            upgradeArray[3].object:GetChildren()[1]:Active(false)

            LevelUp(id, "primary", 3, false)
        end
        -- SECONDARY ABILITY : LEVEL 1
    elseif (upgradeArray[4].object:GetButton():IsPressed() and upgradeArray[4].unlocked == true) then
        if (upgradeArray[4].state == false) then

            if (LevelUp(id, "secondary", 1, true) ~= false) then
                upgradeArray[4].state = true
                upgradeArray[4].object:GetChildren()[1]:Active(true)
                -- Unlock
                upgradeArray[5].unlocked = true
            end
        else
            if upgradeArray[5].state == false then
                upgradeArray[4].state = false
                upgradeArray[4].object:GetChildren()[1]:Active(false)
                upgradeArray[5].unlocked = false

                LevelUp(id, "secondary", 1, false)
            end
        end
        -- SECONDARY ABILITY : LEVEL 2
    elseif (upgradeArray[5].object:GetButton():IsPressed() and upgradeArray[5].unlocked == true) then
        if (upgradeArray[5].state == false) then

            if (LevelUp(id, "secondary", 2, true) ~= false) then
                upgradeArray[5].state = true
                upgradeArray[5].object:GetChildren()[1]:Active(true)
                -- Unlock
                upgradeArray[6].unlocked = true
            end
        else
            if upgradeArray[6].state == false then
                upgradeArray[5].state = false
                upgradeArray[5].object:GetChildren()[1]:Active(false)
                upgradeArray[6].unlocked = false

                LevelUp(id, "secondary", 2, false)
            end
        end
        -- SECONDARY ABILITY : LEVEL 3
    elseif (upgradeArray[6].object:GetButton():IsPressed() and upgradeArray[6].unlocked == true) then
        if (upgradeArray[6].state == false) then

            if (LevelUp(id, "secondary", 3, true) ~= false) then
                upgradeArray[6].state = true
                upgradeArray[6].object:GetChildren()[1]:Active(true)
            end
        else
            upgradeArray[6].state = false
            upgradeArray[6].object:GetChildren()[1]:Active(false)

            LevelUp(id, "secondary", 3, false)
        end
        -- ULTIMATE ABILITY : LEVEL 1
    elseif (upgradeArray[7].object:GetButton():IsPressed() and upgradeArray[7].unlocked == true) then
        if (upgradeArray[7].state == false) then

            if (LevelUp(id, "ultimate", 1, true) ~= false) then
                upgradeArray[7].state = true
                upgradeArray[7].object:GetChildren()[1]:Active(true)
                -- Unlock
                upgradeArray[8].unlocked = true
            end
        else
            if upgradeArray[8].state == false then
                upgradeArray[7].state = false
                upgradeArray[7].object:GetChildren()[1]:Active(false)
                upgradeArray[8].unlocked = false

                LevelUp(id, "ultimate", 1, false)
            end
        end
        -- ULTIMATE ABILITY : LEVEL 2
    elseif (upgradeArray[8].object:GetButton():IsPressed() and upgradeArray[8].unlocked == true) then
        if (upgradeArray[8].state == false) then

            if (LevelUp(id, "ultimate", 2, true) ~= false) then
                upgradeArray[8].state = true
                upgradeArray[8].object:GetChildren()[1]:Active(true)
                -- Unlock
                upgradeArray[9].unlocked = true
            end
        else
            if upgradeArray[9].state == false then
                upgradeArray[8].state = false
                upgradeArray[8].object:GetChildren()[1]:Active(false)
                upgradeArray[9].unlocked = false

                LevelUp(id, "ultimate", 2, false)
            end
        end
        -- ULTIMATE ABILITY : LEVEL 3
    elseif (upgradeArray[9].object:GetButton():IsPressed() and upgradeArray[9].unlocked == true) then
        if (upgradeArray[9].state == false) then

            if (LevelUp(id, "ultimate", 3, true) ~= false) then
                upgradeArray[9].state = true
                upgradeArray[9].object:GetChildren()[1]:Active(true)
            end
        else
            upgradeArray[9].state = false
            upgradeArray[9].object:GetChildren()[1]:Active(false)

            LevelUp(id, "ultimate", 3, false)
        end
        -- PASSIVE ABILITY : LEVEL 1
    elseif (upgradeArray[10].object:GetButton():IsPressed() and upgradeArray[10].unlocked == true) then
        if (upgradeArray[10].state == false) then

            if (LevelUp(id, "passive", 1, true) ~= false) then
                upgradeArray[10].state = true
                upgradeArray[10].object:GetChildren()[1]:Active(true)
                -- Unlock
                upgradeArray[11].unlocked = true
            end
        else
            if upgradeArray[11].state == false then
                upgradeArray[10].state = false
                upgradeArray[10].object:GetChildren()[1]:Active(false)
                upgradeArray[11].unlocked = false

                LevelUp(id, "passive", 1, false)
            end
        end
        -- PASSIVE ABILITY : LEVEL 2
    elseif (upgradeArray[11].object:GetButton():IsPressed() and upgradeArray[11].unlocked == true) then
        if (upgradeArray[11].state == false) then

            if (LevelUp(id, "passive", 2, true) ~= false) then
                upgradeArray[11].state = true
                upgradeArray[11].object:GetChildren()[1]:Active(true)
                -- Unlock
                upgradeArray[12].unlocked = true
            end
        else
            if upgradeArray[12].state == false then
                upgradeArray[11].state = false
                upgradeArray[11].object:GetChildren()[1]:Active(false)
                upgradeArray[12].unlocked = false

                LevelUp(id, "passive", 2, false)
            end
        end
        -- PASSIVE ABILITY : LEVEL 3
    elseif (upgradeArray[12].object:GetButton():IsPressed() and upgradeArray[12].unlocked == true) then
        if (upgradeArray[12].state == false) then

            if (LevelUp(id, "passive", 3, true) ~= false) then
                upgradeArray[12].state = true
                upgradeArray[12].object:GetChildren()[1]:Active(true)
            end
        else
            upgradeArray[12].state = false
            upgradeArray[12].object:GetChildren()[1]:Active(false)

            LevelUp(id, "passive", 3, false)
        end
    end
end

function EventHandler(key, fields)
    if (key == "TransitionedFromLastCharacter") then -- fields[1] -> go;
        id = fields[1]
        changedCharacter = true
        UpdateUI()
    elseif (key == "Gamestate_Loaded") then
        UpdateState()
    end
end

------------ Dialogue ------------
function UpdateUI()

    if (changedCharacter == true) then
        if (id == 0) then
            skillUiArray = {1, 2, 3, 101}

            upgradeArray = {upgradeButton1Z, upgradeButton2Z, upgradeButton3Z, upgradeButton4Z, upgradeButton5Z,
                            upgradeButton6Z, upgradeButton7Z, upgradeButton8Z, upgradeButton9Z, upgradeButton10Z,
                            upgradeButton11Z, upgradeButton12Z}
        elseif (id == 1) then
            skillUiArray = {4, 5, 6, 102}

            upgradeArray = {upgradeButton1N, upgradeButton2N, upgradeButton3N, upgradeButton4N, upgradeButton5N,
                            upgradeButton6N, upgradeButton7N, upgradeButton8N, upgradeButton9N, upgradeButton10N,
                            upgradeButton11N, upgradeButton12N}
        elseif (id == 2) then
            skillUiArray = {7, 8, 9, 103}

            upgradeArray = {upgradeButton1O, upgradeButton2O, upgradeButton3O, upgradeButton4O, upgradeButton5O,
                            upgradeButton6O, upgradeButton7O, upgradeButton8O, upgradeButton9O, upgradeButton10O,
                            upgradeButton11O, upgradeButton12O}
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
    if (index == 1) then
        dialogueGo:Active(true)
        dialog = GetTransString("description1", skillUiArray[1])
        dialog1 = GetTransString("description2", skillUiArray[1])
        dialog2 = GetTransString("description3", skillUiArray[1])
        dialog3 = GetTransString("description4", skillUiArray[1])
        dialog4 = GetTransString("description5", skillUiArray[1])
        dialog5 = GetTransString("description6", skillUiArray[1])
    elseif (index == 2) then
        dialogueGo:Active(true)
        dialog = GetTransString("description1", skillUiArray[2])
        dialog1 = GetTransString("description2", skillUiArray[2])
        dialog2 = GetTransString("description3", skillUiArray[2])
        dialog3 = GetTransString("description4", skillUiArray[2])
        dialog4 = GetTransString("description5", skillUiArray[2])
        dialog5 = GetTransString("description6", skillUiArray[2])
    elseif (index == 3) then
        dialogueGo:Active(true)
        dialog = GetTransString("description1", skillUiArray[3])
        dialog1 = GetTransString("description2", skillUiArray[3])
        dialog2 = GetTransString("description3", skillUiArray[3])
        dialog3 = GetTransString("description4", skillUiArray[3])
        dialog4 = GetTransString("description5", skillUiArray[3])
        dialog5 = GetTransString("description6", skillUiArray[3])
    elseif (index == 4) then
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
    SetVariable(newSpice, "UI_GameState.lua", "spice", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
end

function SetSpiceAmount(amount)
    SpiceAmountText:GetText():SetTextValue(amount)
    SetVariable(amount, "UI_GameState.lua", "spice", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
end

function LevelUp(characterID, skill, skillLevel, direction)

    local characterName

    if (characterID == 0) then
        characterName = "zhib"
    elseif (characterID == 1) then
        characterName = "nerala"
    elseif (characterID == 2) then
        characterName = "omozra"
    end

    local characterAbilityName
    local abilityCost = calcAbilityCost(skillLevel)

    if (skill == "primary") then
        characterAbilityName = characterName .. "_primary_level"
    elseif (skill == "secondary") then
        characterAbilityName = characterName .. "_secondary_level"
    elseif (skill == "ultimate") then
        abilityCost = calcAbilityCost(skillLevel, skill)
        characterAbilityName = characterName .. "_ultimate_level"
    elseif (skill == "passive") then
        abilityCost = calcAbilityCost(skillLevel, skill)
        characterAbilityName = characterName .. "_passive_level"
    end

    if (direction == true) then
        if (GetVariable("UI_GameState.lua", "spice", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) < abilityCost) then
            return false
        end
    elseif (direction == false) then
        abilityCost = -(abilityCost)
        skillLevel = skillLevel - 1
    end

    SpiceCost(abilityCost)
    SetVariable(skillLevel, "UI_GameState.lua", characterAbilityName, INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)

    DispatchEvent("Save_Game", {})
end

function UpdateState()

    local characterName
    local characterAbilityName

    for i = 1, 3 do
        if (i == 1) then
            characterName = "zhib"
            upgradeArray = {upgradeButton1Z, upgradeButton2Z, upgradeButton3Z, upgradeButton4Z, upgradeButton5Z,
                            upgradeButton6Z, upgradeButton7Z, upgradeButton8Z, upgradeButton9Z, upgradeButton10Z,
                            upgradeButton11Z, upgradeButton12Z}
        elseif (i == 2) then
            characterName = "nerala"
            upgradeArray = {upgradeButton1N, upgradeButton2N, upgradeButton3N, upgradeButton4N, upgradeButton5N,
                            upgradeButton6N, upgradeButton7N, upgradeButton8N, upgradeButton9N, upgradeButton10N,
                            upgradeButton11N, upgradeButton12N}
        elseif (i == 3) then
            characterName = "omozra"
            upgradeArray = {upgradeButton1O, upgradeButton2O, upgradeButton3O, upgradeButton4O, upgradeButton5O,
                            upgradeButton6O, upgradeButton7O, upgradeButton8O, upgradeButton9O, upgradeButton10O,
                            upgradeButton11O, upgradeButton12O}
        end

        for j = 1, #upgradeArray do

            if (j < 4) then
                characterAbilityName = characterName .. "_primary_level"
                offset = 0
            elseif (j < 7 and j > 3) then
                characterAbilityName = characterName .. "_secondary_level"
                offset = 3
            elseif (j < 10 and j > 6) then
                characterAbilityName = characterName .. "_ultimate_level"
                offset = 6
            else
                characterAbilityName = characterName .. "_passive_level"
                offset = 9
            end

            if ((offset + GetVariable("UI_GameState.lua", characterAbilityName, INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)) ~=
                (offset)) then
                upgradeArray[offset +
                    GetVariable("UI_GameState.lua", characterAbilityName, INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)]
                    .unlocked = true
                upgradeArray[offset +
                    GetVariable("UI_GameState.lua", characterAbilityName, INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)].state =
                    true
            end

            if (GetVariable("UI_GameState.lua", characterAbilityName, INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) == 3) then
                upgradeArray[offset +
                    GetVariable("UI_GameState.lua", characterAbilityName, INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) - 1]
                    .unlocked = true
                upgradeArray[offset +
                    GetVariable("UI_GameState.lua", characterAbilityName, INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) - 1]
                    .state = true
                upgradeArray[offset +
                    GetVariable("UI_GameState.lua", characterAbilityName, INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) - 2]
                    .unlocked = true
                upgradeArray[offset +
                    GetVariable("UI_GameState.lua", characterAbilityName, INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) - 2]
                    .state = true
            elseif (GetVariable("UI_GameState.lua", characterAbilityName, INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) == 2) then
                upgradeArray[offset +
                    GetVariable("UI_GameState.lua", characterAbilityName, INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) - 1]
                    .unlocked = true
                upgradeArray[offset +
                    GetVariable("UI_GameState.lua", characterAbilityName, INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) - 1]
                    .state = true
                upgradeArray[offset +
                    GetVariable("UI_GameState.lua", characterAbilityName, INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) + 1]
                    .unlocked = true
                upgradeArray[offset +
                    GetVariable("UI_GameState.lua", characterAbilityName, INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) + 1]
                    .state = false
            elseif (GetVariable("UI_GameState.lua", characterAbilityName, INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) == 1) then
                upgradeArray[offset +
                    GetVariable("UI_GameState.lua", characterAbilityName, INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) + 1]
                    .unlocked = true
            end
        end
    end
end

print("sceneTransitionManager.lua compiled successfully!")
