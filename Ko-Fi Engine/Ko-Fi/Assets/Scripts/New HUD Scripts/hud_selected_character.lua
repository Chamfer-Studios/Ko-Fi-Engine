local path = "Assets/Scenes/SceneTransitionUI/sceneTransition.json"

character = {
    Zhib = 1,
    Nerala = 2,
    Omozra = 3,
    None = 4
}

Ability = {
    Primary = 1,
    Secondary = 2,
    Ultimate = 3
}

AbilityStatus = {
    Normal = 1,
    Active = 2,
    Cooldown = 3,
    Using = 4,
    Pickable = 5,
    Disabled = 6
}

zhibAbilities = {
    AbilityPrimary = AbilityStatus.Normal,
    AbilitySecondary = AbilityStatus.Normal,
    AbilityUltimate = AbilityStatus.Normal
}

neralaAbilities = {
    AbilityPrimary = AbilityStatus.Normal,
    AbilitySecondary = AbilityStatus.Normal,
    AbilityUltimate = AbilityStatus.Normal
}

omozraAbilities = {
    AbilityPrimary = AbilityStatus.Normal,
    AbilitySecondary = AbilityStatus.Normal,
    AbilityUltimate = AbilityStatus.Normal
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

    blinkTimerQ = nil
    blinkTimerW = nil
    blinkTimerE = nil
    blink = 0.4

    takeDamageBorder = Find("Take Damage Border")
    takeDamageTimer = nil
    takeDamageDuration = 0.5

    currentCharges = GetVariable("Omozra.lua", "currentCharges", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
    omozra_frame1 = Find("Omozra Charge Frame 1")
    omozra_frame2 = Find("Omozra Charge Frame 2")
    omozra_frame3 = Find("Omozra Charge Frame 3")
    omozra_frame4 = Find("Omozra Charge Frame 4")
    omozra_frame5 = Find("Omozra Charge Frame 5")
    omozra_frame6 = Find("Omozra Charge Frame 6")
    omozra_frame1.active = true
    omozra_frame2.active = true
    omozra_frame3.active = true
    omozra_frame4.active = true
    omozra_frame5.active = true
    omozra_frame6.active = true
    omozra_full1 = Find("Omozra Charge Full 1")
    omozra_full2 = Find("Omozra Charge Full 2")
    omozra_full3 = Find("Omozra Charge Full 3")
    omozra_full4 = Find("Omozra Charge Full 4")
    omozra_full5 = Find("Omozra Charge Full 5")
    omozra_full6 = Find("Omozra Charge Full 6")
    omozra_full1.active = true
    omozra_full2.active = true
    omozra_full3.active = true
    omozra_full4.active = true
    omozra_full5.active = true
    omozra_full6.active = true
    omozra_blink1 = Find("Omozra Charge Blink 1")
    omozra_blink2 = Find("Omozra Charge Blink 2")
    omozra_blink3 = Find("Omozra Charge Blink 3")
    omozra_blink4 = Find("Omozra Charge Blink 4")
    omozra_blink5 = Find("Omozra Charge Blink 5")
    omozra_blink6 = Find("Omozra Charge Blink 6")
    omozra_blink1.active = false
    omozra_blink2.active = false
    omozra_blink3.active = false
    omozra_blink4.active = false
    omozra_blink5.active = false
    omozra_blink6.active = false

    spiceAmount = GetVariable("GameState.lua", "spiceAmount", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
    spiceAdditional = 0
    spiceAdditionalDuration = 2
    spiceFrame = Find("Spice Frame")
    spiceDefault = Find("Spice Mask Default")
    spiceMask = Find("Spice Mask Blink")
    spiceText = Find("Spice Text New")
    spiceTextAdditional = Find("Spice Text Additional")
    spiceTextAdditional.active = false
    spiceTextSizeFactor = 30
    spiceTextPosFactor = spiceTextSizeFactor / 2
    spiceTextSizeY = spiceText:GetTransform2D():GetSize().y
    spiceTextPosY = spiceText:GetTransform2D():GetPosition().y
    spiceTextPosX = spiceText:GetTransform2D():GetPosition().x + spiceTextPosFactor
    spiceText:GetText():SetColorRGB(255, 255, 255)

    once = false
end

function Update(dt)
    if once == false then
        once = true
        SetText(spiceAmount)
    end

    currentCharacterId = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)

    AbilityMasking(dt)

    OmozraCharges(dt)

    ManageTimers(dt)

    CheckSelectedCharacterButtons()

    CurrentCharacterDrawing()

    if (skillButtonQ ~= nil and skillButtonW ~= nil and skillButtonE ~= nil and SkillDescription ~= nil) then
        if (skillButtonQ:GetButton():IsIdle() and skillButtonW:GetButton():IsIdle() and
            skillButtonE:GetButton():IsIdle()) then
            SkillDescription:Active(false)
        end
    end

    TakeDamage(dt)
end

function SetText(number)
    local digitCount = NumberOfDigits(number)
    local newSizeX = digitCount * spiceTextSizeFactor
    spiceText:GetTransform2D():SetSize(float2.new(newSizeX, spiceTextSizeY))
    spiceText:GetTransform2D():SetPosition(float2.new(spiceTextPosX - spiceTextPosFactor * digitCount, spiceTextPosY))

    local textSpice = tostring(number)
    spiceText:GetText():SetTextValue(textSpice)
end

function SetTextAdditional(number, action)
    local str = ""
    if action == "add" then
        str = "+" .. number
    elseif action == "minus" then
        str = "-" .. number
    else
        str = "+" .. number
    end
    spiceTextAdditional:GetTransform2D():SetSize(float2.new(#str * spiceTextSizeFactor, spiceTextSizeY))
    spiceTextAdditional:GetText():SetTextValue(str)
end

function AbilityMasking(dt)
    -- Zhib
    if currentCharacterId == 1 then
        if zhibAbilities.AbilityPrimary == 1 then
            activeMaskQ.active = false
            cooldownMaskQ.active = false
            usingMaskQ.active = false
            pickableMaskQ.active = false
            disabledMaskQ.active = false
        elseif zhibAbilities.AbilityPrimary == 2 then
            activeMaskQ.active = true
            cooldownMaskQ.active = false
            usingMaskQ.active = false
            pickableMaskQ.active = false
            disabledMaskQ.active = false
        elseif zhibAbilities.AbilityPrimary == 3 then
            activeMaskQ.active = false
            cooldownMaskQ.active = true
            usingMaskQ.active = false
            pickableMaskQ.active = false
            disabledMaskQ.active = false
        elseif zhibAbilities.AbilityPrimary == 4 then
            cooldownMaskQ.active = false
            pickableMaskQ.active = false
            disabledMaskQ.active = false

            if blinkTimerQ == nil then
                blinkTimerQ = 0.0
                usingMaskW.active = true
                activeMaskW.active = false
            end
            blinkTimerQ = blinkTimerQ + dt
            if blinkTimerQ > blink then
                usingMaskQ.active = not usingMaskQ.active
                activeMaskQ.active = not activeMaskQ.active
                blinkTimerQ = 0.0
            end
        elseif zhibAbilities.AbilityPrimary == 5 then
            activeMaskQ.active = false
            cooldownMaskQ.active = false
            usingMaskQ.active = false
            pickableMaskQ.active = true
            disabledMaskQ.active = false
        elseif zhibAbilities.AbilityPrimary == 6 then
            activeMaskQ.active = false
            cooldownMaskQ.active = false
            usingMaskQ.active = false
            pickableMaskQ.active = false
            disabledMaskQ.active = true
        end
        if zhibAbilities.AbilitySecondary == 1 then
            activeMaskW.active = false
            cooldownMaskW.active = false
            usingMaskW.active = false
            pickableMaskW.active = false
            disabledMaskW.active = false
        elseif zhibAbilities.AbilitySecondary == 2 then
            activeMaskW.active = true
            cooldownMaskW.active = false
            usingMaskW.active = false
            pickableMaskW.active = false
            disabledMaskW.active = false
        elseif zhibAbilities.AbilitySecondary == 3 then
            activeMaskW.active = false
            cooldownMaskW.active = true
            usingMaskW.active = false
            pickableMaskW.active = false
            disabledMaskW.active = false
        elseif zhibAbilities.AbilitySecondary == 4 then
            cooldownMaskW.active = false
            pickableMaskW.active = false
            disabledMaskW.active = false

            if blinkTimerW == nil then
                blinkTimerW = 0.0
                usingMaskW.active = true
                activeMaskW.active = false
            end
            blinkTimerW = blinkTimerW + dt
            if blinkTimerW > blink then
                usingMaskW.active = not usingMaskW.active
                activeMaskW.active = not activeMaskW.active
                blinkTimerW = 0.0
            end
        elseif zhibAbilities.AbilitySecondary == 5 then
            activeMaskW.active = false
            cooldownMaskW.active = false
            usingMaskW.active = false
            pickableMaskW.active = true
            disabledMaskW.active = false
        elseif zhibAbilities.AbilitySecondary == 6 then
            activeMaskW.active = false
            cooldownMaskW.active = false
            usingMaskW.active = false
            pickableMaskW.active = false
            disabledMaskW.active = true
        end
        if zhibAbilities.AbilityUltimate == 1 then
            activeMaskE.active = false
            cooldownMaskE.active = false
            usingMaskE.active = false
            pickableMaskE.active = false
            disabledMaskE.active = false
        elseif zhibAbilities.AbilityUltimate == 2 then
            activeMaskE.active = true
            cooldownMaskE.active = false
            usingMaskE.active = false
            pickableMaskE.active = false
            disabledMaskE.active = false
        elseif zhibAbilities.AbilityUltimate == 3 then
            activeMaskE.active = false
            cooldownMaskE.active = true
            usingMaskE.active = false
            pickableMaskE.active = false
            disabledMaskE.active = false
        elseif zhibAbilities.AbilityUltimate == 4 then
            cooldownMaskE.active = false
            pickableMaskE.active = false
            disabledMaskE.active = false

            if blinkTimerE == nil then
                blinkTimerE = 0.0
                usingMaskW.active = true
                activeMaskW.active = false
            end
            blinkTimerE = blinkTimerE + dt
            if blinkTimerE > blink then
                usingMaskE.active = not usingMaskE.active
                activeMaskE.active = not activeMaskE.active
                blinkTimerE = 0.0
            end
        elseif zhibAbilities.AbilityUltimate == 5 then
            activeMaskE.active = false
            cooldownMaskE.active = false
            usingMaskE.active = false
            pickableMaskE.active = true
            disabledMaskE.active = false
        elseif zhibAbilities.AbilityUltimate == 6 then
            activeMaskE.active = false
            cooldownMaskE.active = false
            usingMaskE.active = false
            pickableMaskE.active = false
            disabledMaskE.active = true
        end
    elseif currentCharacterId == 2 then
        if neralaAbilities.AbilityPrimary == 1 then
            activeMaskQ.active = false
            cooldownMaskQ.active = false
            usingMaskQ.active = false
            pickableMaskQ.active = false
            disabledMaskQ.active = false
        elseif neralaAbilities.AbilityPrimary == 2 then
            activeMaskQ.active = true
            cooldownMaskQ.active = false
            usingMaskQ.active = false
            pickableMaskQ.active = false
            disabledMaskQ.active = false
        elseif neralaAbilities.AbilityPrimary == 3 then
            activeMaskQ.active = false
            cooldownMaskQ.active = true
            usingMaskQ.active = false
            pickableMaskQ.active = false
            disabledMaskQ.active = false
        elseif neralaAbilities.AbilityPrimary == 4 then
            cooldownMaskQ.active = false
            pickableMaskQ.active = false
            disabledMaskQ.active = false

            if blinkTimerQ == nil then
                blinkTimerQ = 0.0
                usingMaskW.active = true
                activeMaskW.active = false
            end
            blinkTimerQ = blinkTimerQ + dt
            if blinkTimerQ > blink then
                usingMaskQ.active = not usingMaskQ.active
                activeMaskQ.active = not activeMaskQ.active
                blinkTimerQ = 0.0
            end
        elseif neralaAbilities.AbilityPrimary == 5 then
            activeMaskQ.active = false
            cooldownMaskQ.active = false
            usingMaskQ.active = false
            pickableMaskQ.active = true
            disabledMaskQ.active = false
        elseif neralaAbilities.AbilityPrimary == 6 then
            activeMaskQ.active = false
            cooldownMaskQ.active = false
            usingMaskQ.active = false
            pickableMaskQ.active = false
            disabledMaskQ.active = true
        end
        if neralaAbilities.AbilitySecondary == 1 then
            activeMaskW.active = false
            cooldownMaskW.active = false
            usingMaskW.active = false
            pickableMaskW.active = false
            disabledMaskW.active = false
        elseif neralaAbilities.AbilitySecondary == 2 then
            activeMaskW.active = true
            cooldownMaskW.active = false
            usingMaskW.active = false
            pickableMaskW.active = false
            disabledMaskW.active = false
        elseif neralaAbilities.AbilitySecondary == 3 then
            activeMaskW.active = false
            cooldownMaskW.active = true
            usingMaskW.active = false
            pickableMaskW.active = false
            disabledMaskW.active = false
        elseif neralaAbilities.AbilitySecondary == 4 then
            cooldownMaskW.active = false
            pickableMaskW.active = false
            disabledMaskW.active = false

            if blinkTimerW == nil then
                blinkTimerW = 0.0
                usingMaskW.active = true
                activeMaskW.active = false
            end
            blinkTimerW = blinkTimerW + dt
            if blinkTimerW > blink then
                usingMaskW.active = not usingMaskW.active
                activeMaskW.active = not activeMaskW.active
                blinkTimerW = 0.0
            end
        elseif neralaAbilities.AbilitySecondary == 5 then
            activeMaskW.active = false
            cooldownMaskW.active = false
            usingMaskW.active = false
            pickableMaskW.active = true
            disabledMaskW.active = false
        elseif neralaAbilities.AbilitySecondary == 6 then
            activeMaskW.active = false
            cooldownMaskW.active = false
            usingMaskW.active = false
            pickableMaskW.active = false
            disabledMaskW.active = true
        end
        if neralaAbilities.AbilityUltimate == 1 then
            activeMaskE.active = false
            cooldownMaskE.active = false
            usingMaskE.active = false
            pickableMaskE.active = false
            disabledMaskE.active = false
        elseif neralaAbilities.AbilityUltimate == 2 then
            activeMaskE.active = true
            cooldownMaskE.active = false
            usingMaskE.active = false
            pickableMaskE.active = false
            disabledMaskE.active = false
        elseif neralaAbilities.AbilityUltimate == 3 then
            activeMaskE.active = false
            cooldownMaskE.active = true
            usingMaskE.active = false
            pickableMaskE.active = false
            disabledMaskE.active = false
        elseif neralaAbilities.AbilityUltimate == 4 then
            cooldownMaskE.active = false
            pickableMaskE.active = false
            disabledMaskE.active = false

            if blinkTimerE == nil then
                blinkTimerE = 0.0
                usingMaskW.active = true
                activeMaskW.active = false
            end
            blinkTimerE = blinkTimerE + dt
            if blinkTimerE > blink then
                usingMaskE.active = not usingMaskE.active
                activeMaskE.active = not activeMaskE.active
                blinkTimerE = 0.0
            end
        elseif neralaAbilities.AbilityUltimate == 5 then
            activeMaskE.active = false
            cooldownMaskE.active = false
            usingMaskE.active = false
            pickableMaskE.active = true
            disabledMaskE.active = false
        elseif neralaAbilities.AbilityUltimate == 6 then
            activeMaskE.active = false
            cooldownMaskE.active = false
            usingMaskE.active = false
            pickableMaskE.active = false
            disabledMaskE.active = true
        end
    elseif currentCharacterId == 3 then
        if omozraAbilities.AbilityPrimary == 1 then
            activeMaskQ.active = false
            cooldownMaskQ.active = false
            usingMaskQ.active = false
            pickableMaskQ.active = false
            disabledMaskQ.active = false
            if omozraAbilities.AbilitySecondary ~= 2 then
                BlinkOmozraCharges(0, true, dt)
            end
        elseif omozraAbilities.AbilityPrimary == 2 then
            activeMaskQ.active = true
            cooldownMaskQ.active = false
            usingMaskQ.active = false
            pickableMaskQ.active = false
            disabledMaskQ.active = false
            BlinkOmozraCharges(GetVariable("Omozra.lua", "primaryChargeCost", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT),
                false, dt)
        elseif omozraAbilities.AbilityPrimary == 3 then
            activeMaskQ.active = false
            cooldownMaskQ.active = true
            usingMaskQ.active = false
            pickableMaskQ.active = false
            disabledMaskQ.active = false
            if omozraAbilities.AbilitySecondary ~= 2 then
                BlinkOmozraCharges(0, true, dt)
            end
        elseif omozraAbilities.AbilityPrimary == 4 then
            cooldownMaskQ.active = false
            pickableMaskQ.active = false
            disabledMaskQ.active = false

            if blinkTimerQ == nil then
                blinkTimerQ = 0.0
                usingMaskW.active = true
                activeMaskW.active = false
            end
            blinkTimerQ = blinkTimerQ + dt
            if blinkTimerQ > blink then
                usingMaskQ.active = not usingMaskQ.active
                activeMaskQ.active = not activeMaskQ.active
                blinkTimerQ = 0.0
            end
        elseif omozraAbilities.AbilityPrimary == 5 then
            activeMaskQ.active = false
            cooldownMaskQ.active = false
            usingMaskQ.active = false
            pickableMaskQ.active = true
            disabledMaskQ.active = false
        elseif omozraAbilities.AbilityPrimary == 6 then
            activeMaskQ.active = false
            cooldownMaskQ.active = false
            usingMaskQ.active = false
            pickableMaskQ.active = false
            disabledMaskQ.active = true
        end
        if omozraAbilities.AbilitySecondary == 1 then
            activeMaskW.active = false
            cooldownMaskW.active = false
            usingMaskW.active = false
            pickableMaskW.active = false
            disabledMaskW.active = false
            if omozraAbilities.AbilityPrimary ~= 2 then
                BlinkOmozraCharges(0, true, dt)
            end
        elseif omozraAbilities.AbilitySecondary == 2 then
            activeMaskW.active = true
            cooldownMaskW.active = false
            usingMaskW.active = false
            pickableMaskW.active = false
            disabledMaskW.active = false
            BlinkOmozraCharges(GetVariable("Omozra.lua", "secondaryChargeCost", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT),
                false, dt)
        elseif omozraAbilities.AbilitySecondary == 3 then
            activeMaskW.active = false
            cooldownMaskW.active = true
            usingMaskW.active = false
            pickableMaskW.active = false
            disabledMaskW.active = false
            if omozraAbilities.AbilityPrimary ~= 2 then
                BlinkOmozraCharges(0, true, dt)
            end
        elseif omozraAbilities.AbilitySecondary == 4 then
            cooldownMaskW.active = false
            pickableMaskW.active = false
            disabledMaskW.active = false

            if blinkTimerW == nil then
                blinkTimerW = 0.0
                usingMaskW.active = true
                activeMaskW.active = false
            end
            blinkTimerW = blinkTimerW + dt
            if blinkTimerW > blink then
                usingMaskW.active = not usingMaskW.active
                activeMaskW.active = not activeMaskW.active
                blinkTimerW = 0.0
            end
        elseif omozraAbilities.AbilitySecondary == 5 then
            activeMaskW.active = false
            cooldownMaskW.active = false
            usingMaskW.active = false
            pickableMaskW.active = true
            disabledMaskW.active = false
        elseif omozraAbilities.AbilitySecondary == 6 then
            activeMaskW.active = false
            cooldownMaskW.active = false
            usingMaskW.active = false
            pickableMaskW.active = false
            disabledMaskW.active = true
        end
        if omozraAbilities.AbilityUltimate == 1 then
            activeMaskE.active = false
            cooldownMaskE.active = false
            usingMaskE.active = false
            pickableMaskE.active = false
            disabledMaskE.active = false
        elseif omozraAbilities.AbilityUltimate == 2 then
            activeMaskE.active = true
            cooldownMaskE.active = false
            usingMaskE.active = false
            pickableMaskE.active = false
            disabledMaskE.active = false
        elseif omozraAbilities.AbilityUltimate == 3 then
            activeMaskE.active = false
            cooldownMaskE.active = true
            usingMaskE.active = false
            pickableMaskE.active = false
            disabledMaskE.active = false
        elseif omozraAbilities.AbilityUltimate == 4 then
            cooldownMaskE.active = false
            pickableMaskE.active = false
            disabledMaskE.active = false

            if blinkTimerE == nil then
                blinkTimerE = 0.0
                usingMaskW.active = true
                activeMaskW.active = false
            end
            blinkTimerE = blinkTimerE + dt
            if blinkTimerE > blink then
                usingMaskE.active = not usingMaskE.active
                activeMaskE.active = not activeMaskE.active
                blinkTimerE = 0.0
            end
        elseif omozraAbilities.AbilityUltimate == 5 then
            activeMaskE.active = false
            cooldownMaskE.active = false
            usingMaskE.active = false
            pickableMaskE.active = true
            disabledMaskE.active = false
        elseif omozraAbilities.AbilityUltimate == 6 then
            activeMaskE.active = false
            cooldownMaskE.active = false
            usingMaskE.active = false
            pickableMaskE.active = false
            disabledMaskE.active = true
        end
    end
end

function OmozraCharges(dt)
    if currentCharacterId == 3 then
        omozra_frame1.active = true
        omozra_frame2.active = true
        omozra_frame3.active = true
        omozra_frame4.active = true
        omozra_frame5.active = true
        omozra_frame6.active = true
        if currentCharges == 0 then
            omozra_full1.active = false
            omozra_full2.active = false
            omozra_full3.active = false
            omozra_full4.active = false
            omozra_full5.active = false
            omozra_full6.active = false
        elseif currentCharges == 1 then
            omozra_full1.active = true
            omozra_full2.active = false
            omozra_full3.active = false
            omozra_full4.active = false
            omozra_full5.active = false
            omozra_full6.active = false
        elseif currentCharges == 2 then
            omozra_full1.active = true
            omozra_full2.active = true
            omozra_full3.active = false
            omozra_full4.active = false
            omozra_full5.active = false
            omozra_full6.active = false
        elseif currentCharges == 3 then
            omozra_full1.active = true
            omozra_full2.active = true
            omozra_full3.active = true
            omozra_full4.active = false
            omozra_full5.active = false
            omozra_full6.active = false
        elseif currentCharges == 4 then
            omozra_full1.active = true
            omozra_full2.active = true
            omozra_full3.active = true
            omozra_full4.active = true
            omozra_full5.active = false
            omozra_full6.active = false
        elseif currentCharges == 5 then
            omozra_full1.active = true
            omozra_full2.active = true
            omozra_full3.active = true
            omozra_full4.active = true
            omozra_full5.active = true
            omozra_full6.active = false
        elseif currentCharges == 6 then
            omozra_full1.active = true
            omozra_full2.active = true
            omozra_full3.active = true
            omozra_full4.active = true
            omozra_full5.active = true
            omozra_full6.active = true
        end
    else
        omozra_full1:Active(false)
        omozra_full2:Active(false)
        omozra_full3:Active(false)
        omozra_full4:Active(false)
        omozra_full5:Active(false)
        omozra_full6:Active(false)
    end
end

function EventHandler(key, fields)

    if key == "Player_Ability" then -- fields[1] -> characterID; fields[2] -> ability n; fields[3] -> ability state
        currentCharacterId = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        characterID = fields[1] -- 1: zhib, 2: Nerala, 3: Omozra
        ability = fields[2] -- 0: canceled, 1: Q, 2: W, 3: E
        abilityState = fields[3] -- 1: normal, 2: active, 3: cooldown, 4: Using, 5: Pickable, 6: Disabled
        HandleCooldowns(fields[4])

        if characterID == 1 then
            if ability == 1 then
                zhibAbilities.AbilityPrimary = abilityState
            elseif ability == 2 then
                zhibAbilities.AbilitySecondary = abilityState
            elseif ability == 3 then
                zhibAbilities.AbilityUltimate = abilityState
            end
        elseif characterID == 2 then
            if ability == 1 then
                neralaAbilities.AbilityPrimary = abilityState
            elseif ability == 2 then
                neralaAbilities.AbilitySecondary = abilityState
            elseif ability == 3 then
                neralaAbilities.AbilityUltimate = abilityState
            end
        elseif characterID == 3 then
            if ability == 1 then
                omozraAbilities.AbilityPrimary = abilityState
            elseif ability == 2 then
                omozraAbilities.AbilitySecondary = abilityState
            elseif ability == 3 then
                omozraAbilities.AbilityUltimate = abilityState
            end
        end
    elseif key == "Player_Health" then -- fields[1] = characterID, fields[2] = currentHP, fields[3] = maxHP
        currentCharacterId = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        ManageHealth(fields[1], fields[2], fields[3])
    elseif key == "Display_Description" then
        DisplayDescription(fields[1], fields[2])
    elseif key == "Enemy_Attack" then
        takeDamageTimer = 0.0
        takeDamageBorder.active = true
    elseif key == "Omozra_Charges" then -- fields[1] == current Charges -- fields[2] == max charges
        currentCharacterId = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        currentCharges = fields[1]
    elseif key == "Spice_Reward" then
        if textTimer ~= nil then
            spiceAdditional = spiceAdditional + fields[1]
        else
            spiceAdditional = fields[1]
        end
        spiceAmount = spiceAmount + fields[1]
        textTimer = 0.0
        spiceTextAdditional.active = true
        SetTextAdditional(spiceAdditional, "add")
    elseif key == "Used_Ultimate" then
        textTimer = 0.0
        spiceTextAdditional.active = true
        spiceAmount = spiceAmount - fields[1]
        spiceAdditional = fields[1]
        SetTextAdditional(spiceAdditional, "minus")
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
        zhibImage.active = true
        zhibKey.active = false
        currentSelectedCharacterFrame = character.Zhib

        -- Nerala on the left, check if available
        if GetVariable("GameState.lua", "neralaAvailable", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true then
            neralaImage:GetTransform2D():SetPosition(float2.new(70, 120)) -- left position
            neralaImage:GetTransform2D():SetSize(float2.new(76.25, 76.25)) -- small size
            neralaImage.active = true
            neralaKey.active = true
            leftCharacterFrame.active = true
            currentLeftCharacterFrame = character.Nerala
        else
            neralaImage.active = false
            neralaKey.active = false
            leftCharacterFrame.active = false
            currentLeftCharacterFrame = character.None
        end

        -- Omozra on the right, check if available
        if GetVariable("GameState.lua", "omozraAvailable", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true then
            omozraImage:GetTransform2D():SetPosition(float2.new(350, 120)) -- right position
            omozraImage:GetTransform2D():SetSize(float2.new(76.25, 76.25)) -- small size
            omozraImage.active = true
            omozraKey.active = true
            rightCharacterFrame.active = true
            currentRightCharacterFrame = character.Omozra
        else
            omozraImage.active = false
            omozraKey.active = false
            rightCharacterFrame.active = false
            currentRightCharacterFrame = character.None
        end

        zhibSkills:SetIsActiveToChildren(zhibSkills:GetChildren(), true) -- activate the skill slots to be visible
        neralaSkills:SetIsActiveToChildren(neralaSkills:GetChildren(), false) -- deactivate the skill slots to be invisible
        omozraSkills:SetIsActiveToChildren(omozraSkills:GetChildren(), false) -- deactivate the skill slots to be invisible
    elseif currentCharacterId == 2 then
        neralaImage:GetTransform2D():SetPosition(float2.new(210, 158)) -- center position
        neralaImage:GetTransform2D():SetSize(float2.new(156.25, 156.25)) -- big size
        neralaImage.active = true
        neralaKey.active = false
        currentSelectedCharacterFrame = character.Nerala

        -- Omozra on the left, check if available
        if GetVariable("GameState.lua", "omozraAvailable", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true then
            omozraImage:GetTransform2D():SetPosition(float2.new(70, 120)) -- left position
            omozraImage:GetTransform2D():SetSize(float2.new(76.25, 76.25)) -- small size
            omozraImage.active = true
            omozraKey.active = true
            leftCharacterFrame.active = true
            currentLeftCharacterFrame = character.Omozra
        else
            omozraImage.active = false
            omozraKey.active = false
            leftCharacterFrame.active = false
            currentLeftCharacterFrame = character.None
        end

        -- Zhib on the right, check if available
        if GetVariable("GameState.lua", "zhibAvailable", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true then
            zhibImage:GetTransform2D():SetPosition(float2.new(350, 120)) -- right position
            zhibImage:GetTransform2D():SetSize(float2.new(76.25, 76.25)) -- small size
            zhibImage.active = true
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
        omozraImage.active = true
        omozraKey.active = false
        currentSelectedCharacterFrame = character.Omozra

        -- Zhib on the left, check if available
        if GetVariable("GameState.lua", "zhibAvailable", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true then
            zhibImage:GetTransform2D():SetPosition(float2.new(70, 120)) -- left position
            zhibImage:GetTransform2D():SetSize(float2.new(76.25, 76.25)) -- small size
            zhibImage.active = true
            zhibKey.active = true
            leftCharacterFrame.active = true
            currentLeftCharacterFrame = character.Zhib
        else
            zhibImage.active = false
            zhibKey.active = false
            leftCharacterFrame.active = false
            currentLeftCharacterFrame = character.None
        end

        -- Nerala on the right, check if available
        if GetVariable("GameState.lua", "neralaAvailable", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true then
            neralaImage:GetTransform2D():SetPosition(float2.new(350, 120)) -- right position
            neralaImage:GetTransform2D():SetSize(float2.new(76.25, 76.25)) -- small size
            neralaImage.active = true
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

    if (textTimer ~= nil) then
        textTimer = textTimer + dt
        if textBlinkTimer == nil then
            textBlinkTimer = 0.0
        end
        textBlinkTimer = textBlinkTimer + dt
        if textBlinkTimer >= 0.4 then
            spiceTextAdditional.active = not spiceTextAdditional.active
        end
        if textTimer >= spiceAdditionalDuration then
            -- spiceAmount = GetVariable("GameState.lua", "spiceAmount", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
            SetText(spiceAmount)
            spiceTextAdditional.active = false
            textTimer = nil
            textBlinkTimer = nil
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

function TakeDamage(dt)
    if takeDamageTimer ~= nil then
        takeDamageTimer = takeDamageTimer + dt
        if takeDamageTimer >= takeDamageDuration then
            takeDamageTimer = nil
            takenDamage = false
            takeDamageBorder.active = false
        end
    end
end

function BlinkOmozraCharges(chargesToBlink, toReset, dt)
    if blinkTimer == nil then
        blinkTimer = 0.0
    end

    if state == nil then
        state = true
    end

    if toReset == true then
        omozra_blink1.active = false
        omozra_blink2.active = false
        omozra_blink3.active = false
        omozra_blink4.active = false
        omozra_blink5.active = false
        omozra_blink6.active = false
    else
        blinkTimer = blinkTimer + dt
        if blinkTimer >= 0.5 then
            if chargesToBlink == 1 then
                omozra_blink1.active = state
                omozra_blink2.active = false
                omozra_blink3.active = false
                omozra_blink4.active = false
                omozra_blink5.active = false
                omozra_blink6.active = false
            elseif chargesToBlink == 2 then
                omozra_blink1.active = state
                omozra_blink2.active = state
                omozra_blink3.active = false
                omozra_blink4.active = false
                omozra_blink5.active = false
                omozra_blink6.active = false
            elseif chargesToBlink == 3 then
                Log("A\n")
                omozra_blink1.active = state
                omozra_blink2.active = state
                omozra_blink3.active = state
                omozra_blink4.active = false
                omozra_blink5.active = false
                omozra_blink6.active = false
            elseif chargesToBlink == 4 then
                omozra_blink1.active = state
                omozra_blink2.active = state
                omozra_blink3.active = state
                omozra_blink4.active = state
                omozra_blink5.active = false
                omozra_blink6.active = false
            elseif chargesToBlink == 5 then
                omozra_blink1.active = state
                omozra_blink2.active = state
                omozra_blink3.active = state
                omozra_blink4.active = state
                omozra_blink5.active = state
                omozra_blink6.active = false
            elseif chargesToBlink == 6 then
                omozra_blink1.active = state
                omozra_blink2.active = state
                omozra_blink3.active = state
                omozra_blink4.active = state
                omozra_blink5.active = state
                omozra_blink6.active = state
            end
            state = not state
            blinkTimer = nil
        else
            -- omozra_blink1.active = false
            -- omozra_blink2.active = false
            -- omozra_blink3.active = false
            -- omozra_blink4.active = false
            -- omozra_blink5.active = false
            -- omozra_blink6.active = false
        end
    end
end

-- Functions
function NumberOfDigits(number)
    local a = number
    if a == 0 then
        return 1
    else
        local digits = 0
        while (a >= 1) do
            a = a / 10;
            digits = digits + 1;
        end
        return digits;
    end
end

print("hud_selected_character.lua compiled succesfully")
Log("hud_selected_character.lua compiled succesfully\n")
