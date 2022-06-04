local path = "Assets/Scenes/SceneTransitionUI/sceneTransition.json"

character = {
    Zhib = 1,
    Nerala = 2,
    Omozra = 3,
    None = 4
}

function Start()
    LoadJsonFile(path)
    skillUiArray = {1, 2, 3, 101}

    currentCharacterId = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)

    selectedCharacterFrame = Find("Selected Character Frame")
    currentSelectedCharacterFrame = character.Zhib
    leftCharacterFrame = Find("Left Character Frame")
    currentLeftCharacterFrame = character.Nerala
    rightCharacterFrame = Find("Right Character Frame")
    currentRightCharacterFrame = character.Omozra

    zhibImage = Find("Zhib Image")
    zhibKey = Find("Zhib Key")
    neralaImage = Find("Nerala Image")
    neralaKey = Find("Nerala Key")
    omozraImage = Find("Omozra Image")
    omozraKey = Find("Omozra Key")

    zhibSkills = Find("Zhib Skills")
    neralaSkills = Find("Nerala Skills")
    omozraSkills = Find("Omozra Skills")
    cooldownMaskQ = Find("Skill Mask CD Q")
    cooldownMaskW = Find("Skill Mask CD W")
    cooldownMaskE = Find("Skill Mask CD E")
    activeMaskQ = Find("Skill Mask Active Q")
    activeMaskW = Find("Skill Mask Active W")
    activeMaskE = Find("Skill Mask Active E")
    disabledMaskQ = Find("Skill Mask Disabled Q")
    disabledMaskW = Find("Skill Mask Disabled W")
    disabledMaskE = Find("Skill Mask Disabled E")
    pickableMaskQ = Find("Skill Mask Pickable Q")
    pickableMaskW = Find("Skill Mask Pickable W")
    pickableMaskE = Find("Skill Mask Pickable E")
    usingMaskQ = Find("Skill Mask Using Q")
    usingMaskW = Find("Skill Mask Using W")
    usingMaskE = Find("Skill Mask Using E")
    SkillDescription = Find("SkillDescription")
    skillButtonQ = Find("Skill Frame Q")
    skillButtonW = Find("Skill Frame W")
    skillButtonE = Find("Skill Frame E")

    line1 = Find("line1")
    line2 = Find("line2")
    line3 = Find("line3")
    line4 = Find("line4")
    line5 = Find("line5")
    line6 = Find("line6")

    hpFrame1 = Find("HP Frame 1")
    hpFrame2 = Find("HP Frame 2")
    hpFrame3 = Find("HP Frame 3")
    hpFrame4 = Find("HP Frame 4")
    hpFill1 = Find("HP Fill 1")
    hpFill2 = Find("HP Fill 2")
    hpFill3 = Find("HP Fill 3")
    hpFill4 = Find("HP Fill 4")

    zhibTimer2 = nil
    zhibTimer2aux = nil
    zhibCooldown2 = GetVariable("Zhib.lua", "secondaryCooldown", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
    zhibTimer3 = nil
    zhibTimer3aux = nil
    zhibCooldown3 = GetVariable("Zhib.lua", "ultimateCooldown", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)

    neralaTimer1 = nil
    neralaTimer1aux = nil
    neralaCooldown1 = GetVariable("Nerala.lua", "primaryCooldown", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
    neralaTimer3 = nil
    neralaTimer3aux = nil
    neralaCooldown3 = GetVariable("Nerala.lua", "ultimateCooldown", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)

    omozraTimer2 = nil
    omozraTimer2aux = nil
    omozraCooldown2 = GetVariable("Omozra.lua", "secondaryCooldown", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
    omozraTimer3 = nil
    omozraTimer3aux = nil
    omozraCooldown3 = GetVariable("Omozra.lua", "ultimateCooldown", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)

    maskSize = cooldownMaskQ:GetTransform2D():GetMask()

    blink = 0.4

    isUsingQ = false
    isUsingW = false
    isUsingE = false
end

function Update(dt)
    currentCharacterId = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)

    ManageTimers(dt)

    CheckSelectedCharacterButtons()

    CurrentCharacterDrawing()

    AbilityStateUsingBlinking(dt)

    if (skillButtonQ ~= nil and skillButtonW ~= nil and skillButtonE ~= nil and SkillDescription ~= nil) then
        if (skillButtonQ:GetButton():IsIdle() and skillButtonW:GetButton():IsIdle() and
            skillButtonE:GetButton():IsIdle()) then
            SkillDescription:Active(false)
        end
    end
end

function EventHandler(key, fields)

    if key == "Player_Ability" then -- fields[1] -> characterID; fields[2] -> ability n; fields[3] -> ability state
        currentCharacterId = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        characterID = fields[1] -- 1: zhib, 2: Nerala, 3: Omozra
        ability = fields[2] -- 0: canceled, 1: Q, 2: W, 3: E
        abilityState = fields[3] -- 1: normal, 2: active, 3: cooldown, 4: Using, 5: Pickable, 6: Disabled
        HandleMasks()
        HandleCooldowns(fields[4])
    elseif key == "Player_Health" then -- fields[1] = characterID, fields[2] = currentHP, fields[3] = maxHP
        currentCharacterId = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        ManageHealth(fields[1], fields[2], fields[3])
    elseif key == "Display_Description" then
        DisplayDescription(fields[1], fields[2])
    end
end

function HandleMasks()
    if currentCharacterId == characterID then
        if ability == 1 then
            if abilityState == 1 then -- Normal
                activeMaskQ.active = false
                cooldownMaskQ.active = false
                usingMaskQ.active = false
                pickableMaskQ.active = false
                disabledMaskQ.active = false
                isUsingQ = false
            elseif abilityState == 2 then -- Active
                activeMaskQ.active = true
                cooldownMaskQ.active = false
                usingMaskQ.active = false
                pickableMaskQ.active = false
                disabledMaskQ.active = false
                isUsingQ = false
            elseif abilityState == 3 then -- CD
                activeMaskQ.active = false
                cooldownMaskQ.active = true
                usingMaskQ.active = false
                pickableMaskQ.active = false
                disabledMaskQ.active = false
                isUsingQ = false
            elseif abilityState == 4 then -- Using
                activeMaskQ.active = false
                cooldownMaskQ.active = false
                usingMaskQ.active = true
                pickableMaskQ.active = false
                disabledMaskQ.active = false
            elseif abilityState == 5 then -- Pickable
                activeMaskQ.active = false
                cooldownMaskQ.active = false
                usingMaskQ.active = false
                pickableMaskQ.active = true
                disabledMaskQ.active = false
                isUsingQ = false
            elseif abilityState == 6 then -- Disabled
                activeMaskQ.active = false
                cooldownMaskQ.active = false
                usingMaskQ.active = false
                pickableMaskQ.active = false
                disabledMaskQ.active = true
                isUsingQ = false
            end

        elseif ability == 2 then
            if abilityState == 1 then -- Normal
                activeMaskW.active = false
                cooldownMaskW.active = false
                usingMaskW.active = false
                pickableMaskW.active = false
                disabledMaskW.active = false
                isUsingW = false
            elseif abilityState == 2 then -- Active
                activeMaskW.active = true
                cooldownMaskW.active = false
                usingMaskW.active = false
                pickableMaskW.active = false
                disabledMaskW.active = false
                isUsingW = false
            elseif abilityState == 3 then -- CD
                activeMaskW.active = false
                cooldownMaskW.active = true
                usingMaskW.active = false
                pickableMaskW.active = false
                disabledMaskW.active = false
                isUsingW = false
            elseif abilityState == 4 then -- Using
                activeMaskW.active = false
                cooldownMaskW.active = false
                usingMaskW.active = true
                pickableMaskW.active = false
                disabledMaskW.active = false
            elseif abilityState == 5 then -- Pickable
                activeMaskW.active = false
                cooldownMaskW.active = false
                usingMaskW.active = false
                pickableMaskW.active = true
                disabledMaskW.active = false
                isUsingW = false
            elseif abilityState == 6 then -- Disabled
                activeMaskW.active = false
                cooldownMaskW.active = false
                usingMaskW.active = false
                pickableMaskW.active = false
                disabledMaskW.active = true
                isUsingW = false
            end
        elseif ability == 3 then
            if abilityState == 1 then -- Normal
                activeMaskE.active = false
                cooldownMaskE.active = false
                usingMaskE.active = false
                pickableMaskE.active = false
                disabledMaskE.active = false
                isUsingE = false
            elseif abilityState == 2 then -- Active
                activeMaskE.active = true
                cooldownMaskE.active = false
                usingMaskE.active = false
                pickableMaskE.active = false
                disabledMaskE.active = false
                isUsingE = false
            elseif abilityState == 3 then -- CD
                activeMaskE.active = false
                cooldownMaskE.active = true
                usingMaskE.active = false
                pickableMaskE.active = false
                disabledMaskE.active = false
                isUsingE = false
            elseif abilityState == 4 then -- Using
                activeMaskE.active = false
                cooldownMaskE.active = false
                usingMaskE.active = true
                pickableMaskE.active = false
                disabledMaskE.active = false
            elseif abilityState == 5 then -- Pickable
                activeMaskE.active = false
                cooldownMaskE.active = false
                usingMaskE.active = false
                pickableMaskE.active = true
                disabledMaskE.active = false
                isUsingE = false
            elseif abilityState == 6 then -- Disabled
                activeMaskE.active = false
                cooldownMaskE.active = false
                usingMaskE.active = false
                pickableMaskE.active = false
                disabledMaskE.active = true
                isUsingE = false
            end
        end
    end
end

function HandleCooldowns(currentTimer)
    if characterID == 1 and abilityState == 3 then
        if ability == 2 then
            zhibTimer2 = currentTimer
            if zhibTimer2aux == nil then
                zhibTimer2aux = zhibCooldown2
            end
        elseif ability == 3 then
            zhibTimer3 = currentTimer
            if zhibTimer3aux == nil then
                zhibTimer3aux = zhibCooldown3
            end
        end
    elseif characterID == 2 and abilityState == 3 then
        if ability == 1 then
            neralaTimer1 = currentTimer
            if neralaTimer1aux == nil then
                neralaTimer1aux = neralaCooldown1
            end
        elseif ability == 3 then
            neralaTimer3 = currentTimer
            if neralaTimer3aux == nil then
                neralaTimer3aux = neralaCooldown3
            end
        end
    elseif characterID == 3 and abilityState == 3 then
        if ability == 2 then
            omozraTimer2 = currentTimer
            if omozraTimer2aux == nil then
                omozraTimer2aux = omozraCooldown2
            end
        elseif ability == 3 then
            omozraTimer3 = currentTimer
            if omozraTimer3aux == nil then
                omozraTimer3aux = omozraCooldown3
            end
        end
    end

end

function ManageHealth(characterId, HP, maxHP)
    if currentCharacterId == characterId then
        if maxHP == 3 then
            hpFrame1:GetTransform2D():SetPosition(float2.new(130, 42))
            hpFrame2:GetTransform2D():SetPosition(float2.new(210, 42))
            hpFrame3:GetTransform2D():SetPosition(float2.new(290, 42))
            hpFrame4.active = false
            hpFill4.active = false
            if HP == 0 then
                hpFill1.active = false
                hpFill2.active = false
                hpFill3.active = false
            elseif HP == 1 then
                hpFill1.active = true
                hpFill2.active = false
                hpFill3.active = false
            elseif HP == 2 then
                hpFill1.active = true
                hpFill2.active = true
                hpFill3.active = false
            elseif HP == 3 then
                hpFill1.active = true
                hpFill2.active = true
                hpFill3.active = true
            end

        elseif maxHP == 4 then
            hpFrame1:GetTransform2D():SetPosition(float2.new(85, 42))
            hpFrame2:GetTransform2D():SetPosition(float2.new(170, 42))
            hpFrame3:GetTransform2D():SetPosition(float2.new(255, 42))
            hpFrame4:GetTransform2D():SetPosition(float2.new(340, 42))
            hpFrame4.active = true
            if HP == 0 then
                hpFill1.active = false
                hpFill2.active = false
                hpFill3.active = false
                hpFill4.active = false
            elseif HP == 1 then
                hpFill1.active = true
                hpFill2.active = false
                hpFill3.active = false
                hpFill4.active = false
            elseif HP == 2 then
                hpFill1.active = true
                hpFill2.active = true
                hpFill3.active = false
                hpFill4.active = false
            elseif HP == 3 then
                hpFill1.active = true
                hpFill2.active = true
                hpFill3.active = true
                hpFill4.active = false
            elseif HP == 4 then
                hpFill1.active = true
                hpFill2.active = true
                hpFill3.active = true
                hpFill4.active = true
            end
        end
    end
end

function CurrentCharacterDrawing()
    if currentCharacterId == 1 then
        zhibImage:GetTransform2D():SetPosition(float2.new(210, 158)) -- center position
        zhibImage:GetTransform2D():SetSize(float2.new(156.25, 156.25)) -- big size
        zhibKey.active = false
        currentSelectedCharacterFrame = character.Zhib

        if GetVariable("GameState.lua", "neralaAvailable", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true then
            neralaImage:GetTransform2D():SetPosition(float2.new(70, 120)) -- left position
            neralaImage:GetTransform2D():SetSize(float2.new(76.25, 76.25)) -- small size
            neralaKey.active = true
            leftCharacterFrame.active = true
            currentLeftCharacterFrame = character.Nerala
        else
            neralaImage.active = false
            neralaKey.active = false
            leftCharacterFrame.active = false
            currentLeftCharacterFrame = character.None
        end

        if GetVariable("GameState.lua", "omozraAvailable", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true then
            omozraImage:GetTransform2D():SetPosition(float2.new(350, 120)) -- right position
            omozraImage:GetTransform2D():SetSize(float2.new(76.25, 76.25)) -- small size
            omozraKey.active = true
            rightCharacterFrame.active = true
            currentRightCharacterFrame = character.Omozra
        else
            omozraImage.active = false
            omozraKey.active = false
            rightCharacterFrame.active = false
            currentRightCharacterFrame = character.Omozra
        end

        zhibSkills:SetIsActiveToChildren(zhibSkills:GetChildren(), true) -- activate the skill slots to be visible
        neralaSkills:SetIsActiveToChildren(neralaSkills:GetChildren(), false) -- deactivate the skill slots to be invisible
        omozraSkills:SetIsActiveToChildren(omozraSkills:GetChildren(), false) -- deactivate the skill slots to be invisible
    elseif currentCharacterId == 2 then
        neralaImage:GetTransform2D():SetPosition(float2.new(210, 158)) -- center position
        neralaImage:GetTransform2D():SetSize(float2.new(156.25, 156.25)) -- big size
        neralaKey.active = false
        currentSelectedCharacterFrame = character.Nerala

        if GetVariable("GameState.lua", "omozraAvailable", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true then
            omozraImage:GetTransform2D():SetPosition(float2.new(70, 120)) -- left position
            omozraImage:GetTransform2D():SetSize(float2.new(76.25, 76.25)) -- small size
            omozraKey.active = true
            leftCharacterFrame.active = true
            currentLeftCharacterFrame = character.Omozra
        else
            omozraImage.active = false
            omozraKey.active = false
            leftCharacterFrame.active = false
            currentLeftCharacterFrame = character.None
        end

        if GetVariable("GameState.lua", "zhibAvailable", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true then
            zhibImage:GetTransform2D():SetPosition(float2.new(350, 120)) -- right position
            zhibImage:GetTransform2D():SetSize(float2.new(76.25, 76.25)) -- small size
            zhibKey.active = true
            rightCharacterFrame.active = true
            currentRightCharacterFrame = character.Zhib
        else
            zhibImage.active = false
            zhibKey.active = false
            rightCharacterFrame.active = false
            currentRightCharacterFrame = character.None
        end

        zhibSkills:SetIsActiveToChildren(zhibSkills:GetChildren(), false) -- deactivate the skill slots to be invisible
        neralaSkills:SetIsActiveToChildren(neralaSkills:GetChildren(), true) -- activate the skill slots to be visible
        omozraSkills:SetIsActiveToChildren(omozraSkills:GetChildren(), false) -- deactivate the skill slots to be invisible
    elseif currentCharacterId == 3 then
        omozraImage:GetTransform2D():SetPosition(float2.new(210, 158)) -- center position
        omozraImage:GetTransform2D():SetSize(float2.new(156.25, 156.25)) -- big size
        omozraKey.active = false
        currentSelectedCharacterFrame = character.Omozra

        if GetVariable("GameState.lua", "zhibAvailable", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true then
            zhibImage:GetTransform2D():SetPosition(float2.new(70, 120)) -- left position
            zhibImage:GetTransform2D():SetSize(float2.new(76.25, 76.25)) -- small size
            zhibKey.active = true
            leftCharacterFrame.active = true
            currentLeftCharacterFrame = character.Zhib
        else
            zhibImage.active = false
            zhibKey.active = false
            leftCharacterFrame.active = false
            currentLeftCharacterFrame = character.None
        end

        if GetVariable("GameState.lua", "neralaAvailable", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true then
            neralaImage:GetTransform2D():SetPosition(float2.new(350, 120)) -- right position
            neralaImage:GetTransform2D():SetSize(float2.new(76.25, 76.25)) -- small size
            neralaKey.active = true
            rightCharacterFrame.active = true
            currentRightCharacterFrame = character.Nerala
        else
            neralaImage.active = false
            neralaKey.active = false
            rightCharacterFrame.active = false
            currentRightCharacterFrame = character.None
        end

        zhibSkills:SetIsActiveToChildren(zhibSkills:GetChildren(), false) -- deactivate the skill slots to be invisible
        neralaSkills:SetIsActiveToChildren(neralaSkills:GetChildren(), false) -- deactivate the skill slots to be invisible
        omozraSkills:SetIsActiveToChildren(omozraSkills:GetChildren(), true) -- activate the skill slots to be visible
    end
end

function AbilityStateUsingBlinking(dt)
    if currentCharacterId == 1 then
        isUsingQ = GetVariable("Zhib.lua", "isUsingQ", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL)
        isUsingW = GetVariable("Zhib.lua", "isUsingW", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL)
        isUsingE = GetVariable("Zhib.lua", "isUsingE", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL)
    elseif currentCharacterId == 2 then
        isUsingQ = GetVariable("Nerala.lua", "isUsingQ", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL)
        isUsingW = GetVariable("Nerala.lua", "isUsingW", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL)
        isUsingE = GetVariable("Nerala.lua", "isUsingE", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL)
    elseif currentCharacterId == 3 then
        isUsingQ = GetVariable("Omozra.lua", "isUsingQ", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL)
        isUsingW = GetVariable("Omozra.lua", "isUsingW", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL)
        isUsingE = GetVariable("Omozra.lua", "isUsingE", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL)
    end

    if isUsingQ ~= false or isUsingW ~= false or isUsingE ~= false then
        if blinkTimer == nil then
            blinkTimer = 0.0
        end
        blinkTimer = blinkTimer + dt
        if blinkTimer >= blink then
            blinkTimer = 0.0
            if isUsingQ == true then
                usingMaskQ.active = not usingMaskQ.active
                activeMaskQ.active = not activeMaskQ.active
            else
                usingMaskQ.active = false
                activeMaskQ.active = false
            end
            if isUsingW == true then
                usingMaskW.active = not usingMaskW.active
                activeMaskW.active = not activeMaskW.active
            else
                usingMaskW.active = false
                activeMaskW.active = false
            end
            if isUsingE == true then
                usingMaskE.active = not usingMaskE.active
                activeMaskE.active = not activeMaskE.active
            else
                usingMaskE.active = false
                activeMaskE.active = false
            end
        end
    else
        if isUsingQ == false then
            usingMaskQ.active = false
            activeMaskQ.active = false
        end
        if isUsingW == false then
            usingMaskW.active = false
            activeMaskW.active = false
        end
        if isUsingE == false then
            usingMaskE.active = false
            activeMaskE.active = false
        end
        blinkTimer = nil
    end
end

function ManageTimers(dt)

    if (zhibTimer2 ~= nil) then
        zhibTimer2 = zhibTimer2 - dt
        zhibTimer2aux = zhibTimer2aux - dt
        if (zhibTimer2 <= 0) then
            zhibTimer2 = nil
            zhibTimer2aux = nil
            cooldownMaskW:GetTransform2D():SetMask(float2.new(maskSize.x, maskSize.y))
        else
            cooldownMaskW:GetTransform2D():SetMask(float2.new(maskSize.x, zhibTimer2aux / zhibCooldown2))
        end
    end
    if (zhibTimer3 ~= nil) then
        zhibTimer3 = zhibTimer3 - dt
        zhibTimer3aux = zhibTimer3aux - dt
        if (zhibTimer3 <= 0) then
            zhibTimer3 = nil
            zhibTimer3aux = nil
            cooldownMaskE:GetTransform2D():SetMask(float2.new(maskSize.x, maskSize.y))
        else
            cooldownMaskE:GetTransform2D():SetMask(float2.new(maskSize.x, zhibTimer3aux / zhibCooldown3))
        end
    end
    if (neralaTimer1 ~= nil) then
        neralaTimer1 = neralaTimer1 - dt
        neralaTimer1aux = neralaTimer1aux - dt
        if (neralaTimer1 <= 0.0) then
            neralaTimer1 = nil
            neralaTimer1aux = nil
            cooldownMaskQ:GetTransform2D():SetMask(float2.new(maskSize.x, maskSize.y))
        else
            cooldownMaskQ:GetTransform2D():SetMask(float2.new(maskSize.x, neralaTimer1aux / neralaCooldown1))
        end

    end
    if (neralaTimer3 ~= nil) then
        neralaTimer3 = neralaTimer3 - dt
        neralaTimer3aux = neralaTimer3aux - dt
        if (neralaTimer3 <= 0) then
            neralaTimer3 = nil
            neralaTimer3aux = nil
            cooldownMaskE:GetTransform2D():SetMask(float2.new(maskSize.x, maskSize.y))
        else
            cooldownMaskE:GetTransform2D():SetMask(float2.new(maskSize.x, neralaTimer3aux / neralaCooldown3))
        end
    end
    if (omozraTimer2 ~= nil) then
        omozraTimer2 = omozraTimer2 - dt
        omozraTimer2aux = omozraTimer2aux - dt
        if (omozraTimer2 <= 0) then
            omozraTimer2 = nil
            omozraTimer2aux = nil
            cooldownMaskW:GetTransform2D():SetMask(float2.new(maskSize.x, maskSize.y))
        else
            cooldownMaskW:GetTransform2D():SetMask(float2.new(maskSize.x, omozraTimer2aux / omozraCooldown2))
        end
    end
    if (omozraTimer3 ~= nil) then
        omozraTimer3 = omozraTimer3 - dt
        omozraTimer3aux = omozraTimer3aux - dt
        if (omozraTimer3 <= 0) then
            omozraTimer3 = nil
            omozraTimer3aux = nil
            cooldownMaskE:GetTransform2D():SetMask(float2.new(maskSize.x, maskSize.y))
        else
            cooldownMaskE:GetTransform2D():SetMask(float2.new(maskSize.x, omozraTimer3aux / omozraCooldown3))
        end
    end
end

function CheckSelectedCharacterButtons()

    if selectedCharacterFrame:GetButton():IsPressed() == true then
        Log("Selected character frame\n")
        DispatchGlobalEvent("Character_Selected", {currentSelectedCharacterFrame})
    end

    if leftCharacterFrame:GetButton():IsPressed() == true then
        Log("Left character frame\n")
        DispatchGlobalEvent("Character_Selected", {currentLeftCharacterFrame})
    end

    if rightCharacterFrame:GetButton():IsPressed() == true then
        Log("Right character frame\n")
        DispatchGlobalEvent("Character_Selected", {currentRightCharacterFrame})
    end
end

function DisplayDescription(buttonIndex, characterId)

    if (SkillDescription ~= nil) then
        SkillDescription:Active(true)
    end
    if (characterId == 1) then
        skillUiArray = {1, 2, 3, 101}
    elseif (characterId == 2) then
        skillUiArray = {4, 5, 6, 102}
    elseif (characterId == 3) then
        skillUiArray = {7, 8, 9, 103}
    end

    if (buttonIndex == 1) then
        dialog = GetTransString("description1", skillUiArray[1])
        dialog1 = GetTransString("description2", skillUiArray[1])
        dialog2 = GetTransString("description3", skillUiArray[1])
        dialog3 = GetTransString("description4", skillUiArray[1])
        dialog4 = GetTransString("description5", skillUiArray[1])
        dialog5 = GetTransString("description6", skillUiArray[1])
    elseif (buttonIndex == 2) then
        dialog = GetTransString("description1", skillUiArray[2])
        dialog1 = GetTransString("description2", skillUiArray[2])
        dialog2 = GetTransString("description3", skillUiArray[2])
        dialog3 = GetTransString("description4", skillUiArray[2])
        dialog4 = GetTransString("description5", skillUiArray[2])
        dialog5 = GetTransString("description6", skillUiArray[2])
    elseif (buttonIndex == 3) then
        dialog = GetTransString("description1", skillUiArray[3])
        dialog1 = GetTransString("description2", skillUiArray[3])
        dialog2 = GetTransString("description3", skillUiArray[3])
        dialog3 = GetTransString("description4", skillUiArray[3])
        dialog4 = GetTransString("description5", skillUiArray[3])
        dialog5 = GetTransString("description6", skillUiArray[3])
    elseif (buttonIndex == 4) then
        dialog = GetTransString("description1", skillUiArray[4])
        dialog1 = GetTransString("description2", skillUiArray[4])
        dialog2 = GetTransString("description3", skillUiArray[4])
        dialog3 = GetTransString("description4", skillUiArray[4])
        dialog4 = GetTransString("description5", skillUiArray[4])
        dialog5 = GetTransString("description6", skillUiArray[4])
    else
        dialog = " "
        dialog1 = " "
        dialog2 = " "
        dialog3 = " "
        dialog4 = " "
        dialog5 = " "
    end

    -- Set Values To The Dialogue
    if (line1 ~= nil and line2 ~= nil and line3 ~= nil and line4 ~= nil and line5 ~= nil and line6 ~= nil) then
        line1:GetText():SetTextValue(dialog)
        line2:GetText():SetTextValue(dialog1)
        line3:GetText():SetTextValue(dialog2)
        line4:GetText():SetTextValue(dialog3)
        line5:GetText():SetTextValue(dialog4)
        line6:GetText():SetTextValue(dialog5)
    end
end

print("UI_AbilitySlot_1.lua compiled succesfully")
Log("UI_AbilitySlot_1.lua compiled succesfully\n")
