knifeHitChance = 100
dartHitChance = 100
neralaAttackHitChance = 100

EnemyDeath = {
    PLAYER_ATTACK = 1,
    KNIFE = 2,
    WEIRDING_WAY = 3,
    MOSQUITO = 4,
    WORM_KILL = 5
}

STATE = {
    UNAWARE = 1,
    SUS = 2,
    AGGRO = 3,
    DEAD = 4,
    VICTORY = 5,
    CORPSE = 6
}

currentState = STATE.UNAWARE

function Start()
    DispatchEvent("Assign_Type", {"Sardaukar"})
    componentSwitch = gameObject:GetAudioSwitch()
    currentTrackID = -1;

end

function Update(dt)
    -- Weirding Way death timer
    if (deathMarkTimer ~= nil) then
        deathMarkTimer = deathMarkTimer + dt
        if (deathMarkTimer >= deathMarkDuration) then
            deathMarkTimer = nil
            DispatchEvent("Enemy_Death", {EnemyDeath.WEIRDING_WAY})
        end
    end
end

function EventHandler(key, fields)
    if key == "Change_State" then -- fields[1] -> oldState; fields[2] -> newState;
        if fields[1] ~= fields[2] then
            currentState = fields[2]
        end
        -- Player basic attack
    elseif key == "Player_Attack" then
        if (fields[1] == gameObject) then
            if fields[2] == 1 then
                DispatchEvent("Enemy_Death", {EnemyDeath.PLAYER_ATTACK})
            elseif fields[2] == 2 then -- Chances only for nerala attack
                if currentState == STATE.UNAWARE or currentState == STATE.AWARE then
                    neralaAttackHitChance = GetVariable("Nerala.lua", "unawareChanceSardAttack",
                        INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
                    math.randomseed(os.time())
                    rng = math.random(100)
                    if (rng <= neralaAttackHitChance) then
                        Log("Culona's attack D100 roll has been " .. rng ..
                                " so the UNAWARE SARDAUKAR enemy is dead! \n")
                        DispatchEvent("Enemy_Death", {EnemyDeath.PLAYER_ATTACK})
                    else
                        Log("Culona's attack D100 roll has been " .. rng ..
                                " so the UNAWARE SARDAUKAR enemy has dodged the attack :( \n")
                        DispatchEvent("Missed", {})
                    end
                elseif currentState == STATE.SUS then
                    neralaAttackHitChance = GetVariable("Nerala.lua", "awareChanceSardAttack",
                        INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
                    math.randomseed(os.time())
                    rng = math.random(100)
                    if (rng <= neralaAttackHitChance) then
                        Log("Culona's attack D100 roll has been " .. rng .. " so the AWARE SARDAUKAR enemy is dead! \n")
                        DispatchEvent("Enemy_Death", {EnemyDeath.PLAYER_ATTACK})
                    else
                        Log("Culona's attack D100 roll has been " .. rng ..
                                " so the AWARE SARDAUKAR enemy has dodged the attack :( \n")
                        DispatchEvent("Missed", {})
                    end
                elseif currentState == STATE.AGGRO then
                    neralaAttackHitChance = GetVariable("Nerala.lua", "aggroChanceSardAttack",
                        INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
                    math.randomseed(os.time())
                    rng = math.random(100)
                    if (rng <= neralaAttackHitChance) then
                        Log("Culona's attack D100 roll has been " .. rng .. " so the AGGRO SARDAUKAR enemy is dead! \n")
                        DispatchEvent("Enemy_Death", {EnemyDeath.PLAYER_ATTACK})
                    else
                        Log("Culona's attack D100 roll has been " .. rng ..
                                " so the AGGRO SARDAUKAR enemy has dodged the attack :( \n")
                        DispatchEvent("Missed", {})
                    end
                end
            end
        end
        -- Zhib knife
    elseif key == "Knife_Hit" then
        if (fields[1] == gameObject) then
            if (currentState == STATE.UNAWARE or currentState == STATE.AWARE) then
                knifeHitChance =
                    GetVariable("Zhib.lua", "unawareChanceSardKnife", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
                math.randomseed(os.time())
                rng = math.random(100)
                if (rng <= knifeHitChance) then
                    Log("Knife's D100 roll has been " .. rng .. " so the UNAWARE SARDAUKAR enemy is dead! \n")
                    DispatchEvent("Enemy_Death", {EnemyDeath.KNIFE})
                else
                    Log("Knife's D100 roll has been " .. rng ..
                            " so the UNAWARE SARDAUKAR enemy has dodged the knife :( \n")
                    DispatchEvent("Missed", {})
                end
            elseif (currentState == STATE.SUS) then
                knifeHitChance = GetVariable("Zhib.lua", "awareChanceSardKnife", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
                math.randomseed(os.time())
                rng = math.random(100)
                if (rng <= knifeHitChance) then
                    Log("Knife's D100 roll has been " .. rng .. " so the AWARE SARDAUKAR enemy is dead! \n")
                    DispatchEvent("Enemy_Death", {EnemyDeath.KNIFE})
                else
                    Log("Knife's D100 roll has been " .. rng ..
                            " so the AWARE SARDAUKAR enemy has dodged the knife :( \n")
                    DispatchEvent("Missed", {})
                end
            elseif (currentState == STATE.AGGRO) then
                knifeHitChance = GetVariable("Zhib.lua", "aggroChanceSardKnife", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
                math.randomseed(os.time())
                rng = math.random(100)
                if (rng <= knifeHitChance) then
                    Log("Knife's D100 roll has been " .. rng .. " so the AGGRO SARDAUKAR enemy is dead! \n")
                    DispatchEvent("Enemy_Death", {EnemyDeath.KNIFE})
                else
                    Log("Knife's D100 roll has been " .. rng ..
                            " so the AGGRO SARDAUKAR enemy has dodged the knife :( \n")
                    DispatchEvent("Missed", {})
                end
            end
        end
        -- Zhib Weirding way
    elseif key == "Death_Mark" then
        if (fields[1] == gameObject) then
            deathMarkTimer = 0.0
            deathMarkDuration = fields[2] * 0.3
        end
        -- Nerala Mosquito
    elseif key == "Mosquito_Hit" then
        if (fields[1] == gameObject) then
            DispatchEvent("Enemy_Death", {EnemyDeath.MOSQUITO})
        end
        -- Omozra ñam ñam
    elseif key == "Sadiq_Update_Target" then -- fields[1] -> target; targeted for (1 -> warning; 2 -> eat; 3 -> spit)
        if (fields[1] == gameObject) then
            if (fields[2] == 1) then
                DispatchEvent("Stop_Movement", {"Worm"})
            elseif (fields[2] == 2) then
                if (currentState == STATE.UNAWARE or currentState == STATE.AWARE) then
                    secondaryHitChance = GetVariable("Omozra.lua", "unawareChanceSardSecondary",
                        INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
                    math.randomseed(os.time())
                    rng = math.random(100)
                    if (rng <= secondaryHitChance) then
                        Log("Ñam ñam's D100 roll has been " .. rng .. " so the UNAWARE SARDAUKAR enemy is dead! \n")
                        DispatchEvent("Enemy_Death", {EnemyDeath.WORM_KILL})
                    else
                        Log("Ñam ñam's D100 roll has been " .. rng ..
                                " so the UNAWARE enemy has dodged the ñam ñam :( \n")
                        DispatchEvent("Missed", {})
                    end
                elseif (currentState == STATE.SUS) then
                    secondaryHitChance = GetVariable("Omozra.lua", "awareChanceSardSecondary",
                        INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
                    math.randomseed(os.time())
                    rng = math.random(100)
                    if (rng <= secondaryHitChance) then
                        Log("Ñam ñam's D100 roll has been " .. rng .. " so the AWARE SARDAUKAR enemy is dead! \n")
                        DispatchEvent("Enemy_Death", {EnemyDeath.WORM_KILL})
                    else
                        Log("Ñam ñam's D100 roll has been " .. rng ..
                                " so the AWARE enemy has dodged the ñam ñam :( \n")
                        DispatchEvent("Missed", {})
                    end
                elseif (currentState == STATE.AGGRO) then
                    secondaryHitChance = GetVariable("Omozra.lua", "aggroChanceSardSecondary",
                        INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
                    math.randomseed(os.time())
                    rng = math.random(100)
                    if (rng <= secondaryHitChance) then
                        Log("Ñam ñam's D100 roll has been " .. rng .. " so the AGGRO SARDAUKAR enemy is dead! \n")
                        DispatchEvent("Enemy_Death", {EnemyDeath.WORM_KILL})
                    else
                        Log("Ñam ñam's D100 roll has been " .. rng ..
                                " so the AGGRO SARDAUKAR enemy has dodged the ñam ñam :( \n")
                        DispatchEvent("Missed", {})
                    end
                end
            elseif fields[2] == 4 then
                DeleteGameObject()
            end
        end
        -- Nerala dart
    elseif key == "Dart_Hit" then
        if (fields[1] == gameObject) then
            if (currentState == STATE.UNAWARE or currentState == STATE.AWARE) then
                dartHitChance =
                    GetVariable("Nerala.lua", "unawareChanceSardDart", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
                math.randomseed(os.time())
                rng = math.random(100)
                if (rng <= dartHitChance) then
                    Log("Dart's D100 roll has been " .. rng .. " so the UNAWARE SARDAUKAR enemy is stunned! \n")
                    DispatchEvent("Dart_Success", {fields[2], fields[3]})
                else
                    Log("Dart's D100 roll has been " .. rng ..
                            " so the UNAWARE SARDAUKAR enemy has dodged the dart :( \n")
                    DispatchEvent("Missed", {})
                end
            elseif (currentState == STATE.SUS) then
                dartHitChance = GetVariable("Nerala.lua", "awareChanceSardDart", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
                math.randomseed(os.time())
                rng = math.random(100)
                if (rng <= dartHitChance) then
                    Log("Dart's D100 roll has been " .. rng .. " so the AWARE SARDAUKAR enemy is stunned! \n")
                    DispatchEvent("Dart_Success", {fields[2], fields[3]})
                else
                    Log("Dart's D100 roll has been " .. rng .. " so the AWARE SARDAUKAR enemy has dodged the dart :( \n")
                    DispatchEvent("Missed", {})
                end
            elseif (currentState == STATE.AGGRO) then
                dartHitChance = GetVariable("Nerala.lua", "aggroChanceSardDart", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
                math.randomseed(os.time())
                rng = math.random(100)
                if (rng <= dartHitChance) then
                    Log("Dart's D100 roll has been " .. rng .. " so the AGGRO SARDAUKAR enemy is stunned! \n")
                    DispatchEvent("Dart_Success", {fields[2], fields[3]})
                else
                    Log("Dart's D100 roll has been " .. rng .. " so the AGGRO SARDAUKAR enemy has dodged the dart :( \n")
                    DispatchEvent("Missed", {})
                end
            end
        end
    end
end

function ChangeTrack(_trackList)
    size = 0
    for i in pairs(_trackList) do
        size = size + 1
    end

    index = math.random(size)

    if (componentSwitch ~= nil) then
        if (currentTrackID ~= -1) then
            componentSwitch:StopTrack(currentTrackID)
        end
        currentTrackID = _trackList[index]
        componentSwitch:PlayTrack(currentTrackID)
    end
end
