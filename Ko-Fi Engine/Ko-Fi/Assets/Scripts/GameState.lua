------------------- Variables --------------------
characterSelected = 1
particleActive = false

-------------------- Methods ---------------------
function Start()
    characters = {Find("Zhib"), Find("Nerala"), Find("Omozra")}
    characterSelectedParticle = Find("Selected Character")
end

-- Called each loop iteration
function Update(dt)

    currentState = GetRuntimeState()
    if (currentState == RuntimeState.PLAYING) then
        if (GetInput(1) == KEY_STATE.KEY_DOWN) then
            local goHovered = GetGameObjectHovered()
            if (goHovered.tag == Tag.PLAYER) then
                if (goHovered:GetName() == "Zhib") then
                    characterSelected = 1
                elseif (goHovered:GetName() == "Nerala") then
                    characterSelected = 2
                elseif (goHovered:GetName() == "Omozra") then
                    characterSelected = 3
                end
            end
            -- Z
        elseif (GetInput(6) == KEY_STATE.KEY_DOWN) then
            if (characterSelected == 1) then
                characterSelected = 0
            else
                characterSelected = 1
            end
            -- X
        elseif (GetInput(8) == KEY_STATE.KEY_DOWN) then
            if (characterSelected == 2) then
                characterSelected = 0
            else
                characterSelected = 2
            end
            -- C
        elseif (GetInput(9) == KEY_STATE.KEY_DOWN) then
            if (characterSelected == 3) then
                characterSelected = 0
            else
                characterSelected = 3
            end
        end
        if (characterSelected ~= 0) then
            playerPos = characters[characterSelected]:GetTransform():GetPosition()
            if (characterSelectedParticle ~= nil) then
                characterSelectedParticle:GetTransform():SetPosition(float3.new(playerPos.x, playerPos.y + 1, playerPos.z))
                if (particleActive == false) then
                    particleActive = true
                    characterSelectedParticle:GetComponentParticle():ResumeParticleSpawn()
                    characterSelectedParticle:GetComponentParticle():SetLoop(true)
                end
            end
        else
            characterSelectedParticle:GetTransform():SetPosition(float3.new(
                characterSelectedParticle:GetTransform():GetPosition().x, -20,
                characterSelectedParticle:GetTransform():GetPosition().z))
        end
    else
        characterSelected = 0
    end
end

function EventHandler(key, fields)
    if key == "Character_Selected" then -- fields[1] -> characterSelected;
        characterSelected = fields[1]
        -- characterSelected = 1
    end
end
--------------------------------------------------

print("GameState.lua compiled succesfully")
