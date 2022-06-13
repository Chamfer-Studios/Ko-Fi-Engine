------------------- Variables --------------------
levelNumber = 1

characterSelected = 1

spiceAmount = 0
startingSpiceAmount = 0
deadAllyPenalization = 2000

particleActive = false
gameOverTime = 5

zhib_hp = 4
zhib_primary_level = 0
zhib_secondary_level = 0
zhib_ultimate_level = 0
zhib_passive_level = 0

nerala_hp = 3
nerala_primary_level = 0
nerala_secondary_level = 0
nerala_ultimate_level = 0
nerala_passive_level = 0

omozra_hp = 3
omozra_primary_level = 0
omozra_secondary_level = 0
omozra_ultimate_level = 0
omozra_passive_level = 0

zhibAvailable = true
neralaAvailable = false
omozraAvailable = false

anyCharacterSelected = true
changedCharacter = false

GodMode = false

--- Inspector Variables
deadAllyPenalizationIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
deadAllyPenalizationIV = InspectorVariable.new("deadAllyPenalization", deadAllyPenalizationIVT, deadAllyPenalization)
NewVariable(deadAllyPenalizationIV)

startingSpiceAmountIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
startingSpiceAmountIV = InspectorVariable.new("startingSpiceAmount", startingSpiceAmountIVT, startingSpiceAmount)
NewVariable(startingSpiceAmountIV)

levelNumberIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
levelNumberIV = InspectorVariable.new("levelNumber", levelNumberIVT, levelNumber)
NewVariable(levelNumberIV)

triggerDialogues = true

-------------------- Methods ---------------------
function Start()
    characters = {Find("Zhib"), Find("Nerala"), Find("Omozra")}
    characterSelectedParticle = Find("Selected Particle")

    isStarting = true
    LoadGame()
end

-- Called each loop iteration
function Update(dt)

    if (GetInput(50) == KEY_STATE.KEY_DOWN) then
        SaveGame()
    end

    if (gameOverTimer ~= nil) then
        if (gameOverTimer < gameOverTime) then
            gameOverTimer = gameOverTimer + dt
        else
            spiceAmount = spiceAmount - deadAllyPenalization

            if (spiceAmount <= 0) then
                spiceAmount = 0
            end

            gameObject:ChangeScene(true, "SceneGameOver")
        end
    end

    currentState = GetRuntimeState()
    if (currentState == RuntimeState.PLAYING) then
        if (GetInput(1) == KEY_STATE.KEY_DOWN and omozraUltimate == false and omozraPrimary == false) then
            local goHovered = GetGameObjectHovered()
            if (goHovered.tag == Tag.PLAYER) then
                if (goHovered:GetName() == "Zhib" and changedCharacter ~= 1 and zhibAvailable == true) then
                    DispatchGlobalEvent("Changed_Character", {characterSelected, 1}) -- From character X to 1
                    characterSelected = 1
                elseif (goHovered:GetName() == "Nerala" and changedCharacter ~= 2 and neralaAvailable == true) then
                    DispatchGlobalEvent("Changed_Character", {characterSelected, 2}) -- From character X to 2
                    characterSelected = 2
                elseif (goHovered:GetName() == "Omozra" and changedCharacter ~= 3 and omozraAvailable == true) then
                    DispatchGlobalEvent("Changed_Character", {characterSelected, 3}) -- From character X to 3
                    characterSelected = 3
                end
            end
            -- Z
        elseif (GetInput(6) == KEY_STATE.KEY_DOWN and zhibAvailable == true) then
            if (characterSelected == 1) then
                DispatchGlobalEvent("Changed_Character", {characterSelected, 0}) -- From character 1 to 0
                characterSelected = 0
                anyCharacterSelected = false
            else
                DispatchGlobalEvent("Changed_Character", {characterSelected, 1}) -- From character X to 1
                characterSelected = 1
            end
            -- X
        elseif (GetInput(8) == KEY_STATE.KEY_DOWN and neralaAvailable == true) then
            if (characterSelected == 2) then
                DispatchGlobalEvent("Changed_Character", {characterSelected, 0}) -- From character 2 to 0
                characterSelected = 0
            else
                DispatchGlobalEvent("Changed_Character", {characterSelected, 2}) -- From character X to 2
                characterSelected = 2
            end
            -- C
        elseif (GetInput(9) == KEY_STATE.KEY_DOWN and omozraAvailable == true) then
            if (characterSelected == 3) then
                DispatchGlobalEvent("Changed_Character", {characterSelected, 0}) -- From character 3 to 0
                characterSelected = 0
            else
                DispatchGlobalEvent("Changed_Character", {characterSelected, 3}) -- From character X to 3
                characterSelected = 3
            end
            -- F3
        elseif (GetInput(42) == KEY_STATE.KEY_DOWN) then
            if (GodMode == false) then
                Log("I AM GOOODDDDDDDD\n")
                GodMode = true
            elseif (GodMode == true) then
                Log("I am not god:(\n")
                GodMode = false
            end
            -- F4
        elseif (GetInput(74) == KEY_STATE.KEY_DOWN) then
            if (triggerDialogues == false) then
                Log("I am triggering dialogues\n")
                triggerDialogues = true
            elseif (triggerDialogues == true) then
                Log("I am not triggering dialogues :(\n")
                triggerDialogues = false
            end
            -- F8
        elseif (GetInput(78) == KEY_STATE.KEY_DOWN) then
            if neralaAvailable == true then
                if (characterSelected == 2) then
                    DispatchGlobalEvent("Changed_Character", {characterSelected, 0}) -- From character 2 to 0
                    characterSelected = 0
                    neralaAvailable = false
                else
                    neralaAvailable = false
                end
                Log("Nerala is not available\n")
            else
                Log("Nerala is available\n")
                neralaAvailable = true
            end
            -- F9
        elseif (GetInput(79) == KEY_STATE.KEY_DOWN) then
            if omozraAvailable == true then
                if (characterSelected == 3) then
                    DispatchGlobalEvent("Changed_Character", {characterSelected, 0}) -- From character 3 to 0
                    characterSelected = 0
                    omozraAvailable = false
                else
                    omozraAvailable = false
                end
                Log("Omozra is not available\n")
            else
                Log("Omozra is available\n")
                omozraAvailable = true
            end
        end
        if (characterSelected ~= 0) then
            if (characterSelected == 2 and characters[4] ~= nil) then
                characterSelected = 4
            end
            playerPos = characters[characterSelected]:GetTransform():GetPosition()
            if (characterSelected == 4) then
                characterSelected = 2
            end
            if (characterSelectedParticle ~= nil) then
                characterSelectedParticle:GetTransform():SetPosition(
                    float3.new(playerPos.x, playerPos.y + 1, playerPos.z))
                if (particleActive == false) then
                    particleActive = true
                    characterSelectedParticle:GetComponentParticle():ResumeParticleSpawn()
                    characterSelectedParticle:GetComponentParticle():SetLoop(true)
                end
            end
            anyCharacterSelected = true
        else
            anyCharacterSelected = false
            characterSelectedParticle:GetTransform():SetPosition(float3.new(
                characterSelectedParticle:GetTransform():GetPosition().x, -20,
                characterSelectedParticle:GetTransform():GetPosition().z))
        end
    else
        characterSelected = 0
    end
    omozraUltimate = false
    omozraPrimary = false
end

function EventHandler(key, fields)
    if key == "Character_Selected" then -- fields[1] -> characterSelected;
        if characterSelected == fields[1] then
            DispatchGlobalEvent("Changed_Character", {characterSelected, 0})
            characterSelected = 0
        else
            Log("Changing to character: " .. fields[1] .. "\n")
            DispatchGlobalEvent("Changed_Character", {characterSelected, fields[1]})
            characterSelected = fields[1]
        end
    elseif (key == "Spice_Reward") then
        spiceAmount = spiceAmount + fields[1]

        if (fields[2] ~= nil) then
            keyJson = "gameobjects_to_delete_lvl" .. levelNumber
            AddGameJsonElement(keyJson, fields[2])
        end
    elseif (key == "Spice_Spawn") then
        deadEnemyPos = fields[1]
        deadEnemyType = fields[2]
    elseif (key == "Spice_Has_Spawned") then
        DispatchGlobalEvent("Spice_Drop", {deadEnemyPos.x, deadEnemyPos.y, deadEnemyPos.z, deadEnemyType})
    elseif (key == "Omozra_Ultimate") then
        omozraUltimate = true
    elseif (key == "Omozra_Primary") then
        omozraPrimary = true
    elseif (key == "Player_Death") then
        if (gameOverTimer == nil) then
            gameOverTimer = 0
        end
    elseif (key == "Enemy_Defeated") then
        keyJson = "gameobjects_to_delete_lvl" .. levelNumber
        AddGameJsonElement(keyJson, fields[1])
    elseif (key == "DialoguePassed") then
        keyJson = "gameobjects_to_delete_lvl" .. levelNumber
        AddGameJsonElement(keyJson, fields[1])
    elseif (key == "Dialogue_Opened") then
        auxGodMode = GodMode
        if (GodMode == false) then
            GodMode = true
        end
    elseif (key == "Dialogue_Closed") then
        GodMode = auxGodMode
    elseif (key == "Mosquito_Spawn") then
        characters[4] = fields[1]
    elseif (key == "Mosquito_Death") then
        characters[4] = nil
    elseif (key == "Disable_Character") then
        if (fields[1] == 1) then
            if (fields[1] == characterSelected) then
                DispatchGlobalEvent("Changed_Character", {characterSelected, 0}) -- From character 3 to 0
                characterSelected = 0
                zhibAvailable = false
            else
                zhibAvailable = false
            end
        elseif (fields[1] == 2) then
            if (fields[1] == characterSelected) then
                DispatchGlobalEvent("Changed_Character", {characterSelected, 0}) -- From character 3 to 0
                characterSelected = 0
                neralaAvailable = false
            else
                neralaAvailable = false
            end
        elseif (fields[1] == 3) then
            if (fields[1] == characterSelected) then
                DispatchGlobalEvent("Changed_Character", {characterSelected, 0}) -- From character 3 to 0
                characterSelected = 0
                omozraAvailable = false
            else
                omozraAvailable = false
            end
        end
    elseif (key == "Enable_Character") then
        if (fields[1] == 1) then
            zhibAvailable = true
        elseif (fields[1] == 2) then
            neralaAvailable = true
        elseif (fields[1] == 3) then
            omozraAvailable = true
        end
        -- Make Sure it is done when unlocking characters (checkpoints)
        SaveGame()
    elseif key == "Last_Checkpoint" then -- fields[1] -> if it has to load
        if (fields[1] == true) then
            LoadGame()
        end
    end
end

function SaveGame()

    SetGameJsonInt("level_progression", levelNumber)

    SetGameJsonInt("spice", spiceAmount)

    -- Zhib Save
    SetGameJsonBool("zhib_available", zhibAvailable)
    SetGameJsonInt("zhib_hp", GetVariable("Zhib.lua", "currentHP", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT))

    zhibPos = GetVariable("Zhib.lua", "gameObject", INSPECTOR_VARIABLE_TYPE.INSPECTOR_GAMEOBJECT)
    zhibPos = zhibPos:GetTransform():GetPosition()
    zhibPos.y = 0.0
    keyJson = "zhib_pos_lvl" .. levelNumber
    SetGameJsonFloat3(keyJson, zhibPos)

    -- Nerala Save
    SetGameJsonBool("nerala_available", neralaAvailable)
    SetGameJsonInt("nerala_hp", GetVariable("Nerala.lua", "currentHP", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT))

    neralaPos = GetVariable("Nerala.lua", "gameObject", INSPECTOR_VARIABLE_TYPE.INSPECTOR_GAMEOBJECT)
    neralaPos = neralaPos:GetTransform():GetPosition()
    neralaPos.y = 0.0
    keyJson = "nerala_pos_lvl" .. levelNumber
    SetGameJsonFloat3(keyJson, neralaPos)

    -- Omozra Save
    SetGameJsonBool("omozra_available", omozraAvailable)
    SetGameJsonInt("omozra_hp", GetVariable("Omozra.lua", "currentHP", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT))
    SetGameJsonInt("omozra_charges", GetVariable("Omozra.lua", "currentCharges", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT))

    omozraPos = GetVariable("Omozra.lua", "gameObject", INSPECTOR_VARIABLE_TYPE.INSPECTOR_GAMEOBJECT)
    omozraPos = omozraPos:GetTransform():GetPosition()
    omozraPos.y = 0.0
    keyJson = "omozra_pos_lvl" .. levelNumber
    SetGameJsonFloat3(keyJson, omozraPos)

    SaveGameState()
end

function LoadGame()
    LoadGameState()

    keyJson = "gameobjects_to_delete_lvl" .. levelNumber
    SizeToDelete = GetGameJsonArraySize(keyJson)

    if (SizeToDelete > 0) then
        SizeToDelete = SizeToDelete - 1
        for i = 0, SizeToDelete do
            uidToDelete = GetGameJsonElement(keyJson, i)
            DeleteGameObjectByUID(uidToDelete)
        end
    end

    spiceAmount = GetGameJsonInt("spice")

    if (spiceAmount == 0) then
        spiceAmount = startingSpiceAmount
    end

    --- Zhib Load
    zhib_primary_level = GetGameJsonInt("zhib_primary_level")
    zhib_secondary_level = GetGameJsonInt("zhib_secondary_level")
    zhib_ultimate_level = GetGameJsonInt("zhib_ultimate_level")
    zhib_passive_level = GetGameJsonInt("zhib_passive_level")

    zhib_hp = GetGameJsonInt("zhib_hp")

    keyJson = "zhib_pos_lvl" .. levelNumber
    zhibPos = GetGameJsonFloat3(keyJson)

    DispatchGlobalEvent("Update_Zhib_State",
        {zhibPos.x, zhibPos.y, zhibPos.z, zhib_primary_level, zhib_secondary_level, zhib_ultimate_level,
         zhib_passive_level, zhib_hp})

    --- Nerala Load
    nerala_primary_level = GetGameJsonInt("nerala_primary_level")
    nerala_secondary_level = GetGameJsonInt("nerala_secondary_level")
    nerala_ultimate_level = GetGameJsonInt("nerala_ultimate_level")
    nerala_passive_level = GetGameJsonInt("nerala_passive_level")

    nerala_hp = GetGameJsonInt("nerala_hp")

    keyJson = "nerala_pos_lvl" .. levelNumber
    neralaPos = GetGameJsonFloat3(keyJson)

    DispatchGlobalEvent("Update_Nerala_State",
        {neralaPos.x, neralaPos.y, neralaPos.z, nerala_primary_level, nerala_secondary_level, nerala_ultimate_level,
         nerala_passive_level, nerala_hp})

    --- Omozra Load
    omozra_primary_level = GetGameJsonInt("omozra_primary_level")
    omozra_secondary_level = GetGameJsonInt("omozra_secondary_level")
    omozra_ultimate_level = GetGameJsonInt("omozra_ultimate_level")
    omozra_passive_level = GetGameJsonInt("omozra_passive_level")

    omozra_hp = GetGameJsonInt("omozra_hp")

    omozra_charges = GetGameJsonInt("omozra_charges")

    keyJson = "omozra_pos_lvl" .. levelNumber
    omozraPos = GetGameJsonFloat3(keyJson)
    DispatchGlobalEvent("Update_Omozra_State",
        {omozraPos.x, omozraPos.y, omozraPos.z, omozra_primary_level, omozra_secondary_level, omozra_ultimate_level,
         omozra_passive_level, omozra_hp, omozra_charges})

    if (levelNumber == 1) then

        zhibAvailable = GetGameJsonBool("zhib_available")
        if (zhibAvailable == false) then
            DispatchEvent("Disable_Character", {1})
        end

        neralaAvailable = GetGameJsonBool("nerala_available")
        if (neralaAvailable == false) then
            DispatchEvent("Disable_Character", {2})
        end

        omozraAvailable = GetGameJsonBool("omozra_available")
        if (omozraAvailable == false) then
            DispatchEvent("Disable_Character", {3})
        end

    elseif (levelNumber == 2) then
        zhib_available = true
        neralaAvailable = true
        omozraAvailable = true
    end

end

print("GameState.lua compiled successfully!")
