------------------- Variables --------------------
characterSelected = 1
spiceAmount = 1000
startingSpiceAmount = 1000
spiceMaxLvl1 = 10000
deadAllyPenalization = 2000
particleActive = false
gameOverTime = 5

nerala_primary_level = 0
nerala_secondary_level = 0
nerala_ultimate_level = 0

zhib_primary_level = 0
zhib_secondary_level = 0
zhib_ultimate_level = 0

omozra_primary_level = 0
omozra_secondary_level = 0
omozra_ultimate_level = 0

changedCharacter = false

GodMode = false

--- Inspector Variables
deadAllyPenalizationIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
deadAllyPenalizationIV = InspectorVariable.new("deadAllyPenalization", deadAllyPenalizationIVT, deadAllyPenalization)
NewVariable(deadAllyPenalizationIV)

startingSpiceAmountIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
startingSpiceAmountIV = InspectorVariable.new("startingSpiceAmount", startingSpiceAmountIVT, startingSpiceAmount)
NewVariable(startingSpiceAmountIV)

-------------------- Methods ---------------------
function Start()
    characters = {Find("Zhib"), Find("Nerala"), Find("Omozra")}
    characterSelectedParticle = Find("Selected Particle")
    staminaBar = Find("Stamina Bar")

    LoadGameState()
    spiceAmount = GetGameJsonInt("spice")

    str = "STARTING Spice Amount " .. spiceAmount .. "\n"
    Log(str)

    if (spiceAmount == 0) then
        spiceAmount = startingSpiceAmount
        str = "Spice Amount " .. spiceAmount .. "\n"
        Log(str)
    end

    nerala_primary_level = GetGameJsonInt("nerala_primary_level")
    nerala_secondary_level = GetGameJsonInt("nerala_secondary_level")
    nerala_ultimate_level = GetGameJsonInt("nerala_ultimate_level")

    zhib_primary_level = GetGameJsonInt("zhib_primary_level")
    zhib_secondary_level = GetGameJsonInt("zhib_secondary_level")
    zhib_ultimate_level = GetGameJsonInt("zhib_ultimate_level")

    omozra_primary_level = GetGameJsonInt("omozra_primary_level")
    omozra_secondary_level = GetGameJsonInt("omozra_secondary_level")
    omozra_ultimate_level = GetGameJsonInt("omozra_ultimate_level")

end

-- Called each loop iteration
function Update(dt)
    if (gameOverTimer ~= nil) then
        if (gameOverTimer < gameOverTime) then
            gameOverTimer = gameOverTimer + dt
        else
            spiceAmount = spiceAmount - deadAllyPenalization

            if (spiceAmount <= 0) then
                spiceAmount = 0
            end

            SetGameJsonInt("spice", spiceAmount)

            str = "Spice Amount " .. spiceAmount .. "\n"
            Log(str)

            SetGameJsonInt("nerala_primary_level", nerala_primary_level)
            SetGameJsonInt("nerala_secondary_level", nerala_secondary_level)
            SetGameJsonInt("nerala_ultimate_level", nerala_ultimate_level)

            SetGameJsonInt("zhib_primary_level", zhib_primary_level)
            SetGameJsonInt("zhib_secondary_level", zhib_secondary_level)
            SetGameJsonInt("zhib_ultimate_level", zhib_ultimate_level)

            SetGameJsonInt("omozra_primary_level", omozra_primary_level)
            SetGameJsonInt("omozra_secondary_level", omozra_secondary_level)
            SetGameJsonInt("omozra_ultimate_level", omozra_ultimate_level)

            SaveGameState()

            gameObject:ChangeScene(true, "SceneGameOver")
        end
    end

    currentState = GetRuntimeState()
    if (currentState == RuntimeState.PLAYING) then
        if (GetInput(1) == KEY_STATE.KEY_DOWN and omozraUltimate == false and omozraPrimary == false) then
            local goHovered = GetGameObjectHovered()
            if (goHovered.tag == Tag.PLAYER) then
                if (goHovered:GetName() == "Zhib" and changedCharacter ~= 1) then
                    DispatchGlobalEvent("Changed_Character", {characterSelected, 1}) -- From character X to 1
                    characterSelected = 1
                elseif (goHovered:GetName() == "Nerala" and changedCharacter ~= 2) then
                    DispatchGlobalEvent("Changed_Character", {characterSelected, 2}) -- From character X to 2
                    characterSelected = 2
                elseif (goHovered:GetName() == "Omozra" and changedCharacter ~= 3) then
                    DispatchGlobalEvent("Changed_Character", {characterSelected, 3}) -- From character X to 3
                    characterSelected = 3
                end
            end
            -- Z
        elseif (GetInput(6) == KEY_STATE.KEY_DOWN) then
            if (characterSelected == 1) then
                DispatchGlobalEvent("Changed_Character", {characterSelected, 0}) -- From character 1 to 0
                characterSelected = 0
            else
                DispatchGlobalEvent("Changed_Character", {characterSelected, 1}) -- From character X to 1
                characterSelected = 1
            end
            -- X
        elseif (GetInput(8) == KEY_STATE.KEY_DOWN) then
            if (characterSelected == 2) then
                DispatchGlobalEvent("Changed_Character", {characterSelected, 0}) -- From character 2 to 0
                characterSelected = 0
            else
                DispatchGlobalEvent("Changed_Character", {characterSelected, 2}) -- From character X to 2
                characterSelected = 2
            end
            -- C
        elseif (GetInput(9) == KEY_STATE.KEY_DOWN) then
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
        end
        if (characterSelected ~= 0) then
            if (characterSelected == 2 and characters[4] ~= nil) then
                characterSelected = 4
            end
            playerPos = characters[characterSelected]:GetTransform():GetPosition()
            if (characterSelected == 4) then
                characterSelected = 2
            end
            staminaBar:GetTransform():SetPosition(float3.new(playerPos.x, playerPos.y + 30, playerPos.z))
            if (characterSelectedParticle ~= nil) then
                characterSelectedParticle:GetTransform():SetPosition(
                    float3.new(playerPos.x, playerPos.y + 1, playerPos.z))
                if (particleActive == false) then
                    particleActive = true
                    characterSelectedParticle:GetComponentParticle():ResumeParticleSpawn()
                    characterSelectedParticle:GetComponentParticle():SetLoop(true)
                end
            end
        else
            staminaBar:GetTransform():SetPosition(float3.new(staminaBar:GetTransform():GetPosition().x, -20,
                staminaBar:GetTransform():GetPosition().z))
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
        characterSelected = fields[1]
        -- characterSelected = 1
    elseif (key == "Spice_Reward") then
        spiceAmount = spiceAmount + fields[1]
        str = "Spice Amount " .. spiceAmount .. "\n"
        Log(str)
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
    end
end
--------------------------------------------------

print("GameState.lua compiled succesfully")
