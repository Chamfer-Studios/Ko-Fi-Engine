STATE = { -- Importat to add changes to the state enum in EnemyController.lua and add them here
    UNAWARE = 1,
    AWARE = 2,
    SUS = 3,
    AGGRO = 4,
    DEAD = 5,
    VICTORY = 6
}

meleeRange = 25.0
rangedAttackRange = 100.0
knifeHitChance = 100

function Start()
    currentState = STATE.UNAWARE
    target = nil
end

function Update()

    if (currentState == STATE.UNAWARE) then

    elseif (currentState == STATE.AWARE) then

    elseif (currentState == STATE.SUS) then

    elseif (currentState == STATE.AGGRO) then
        if (target ~= nil) then
            if (WithinMeleeRange() == true) then
                MeleeAttack()
            elseif (WithinRangedRange() == true) then
                RangedAttack()
            elseif (target ~= nil) then
                MoveTowardsTarget()
            else
                -- Keep doing whatever it was doing
            end
        end
    elseif (currentState == STATE.DEAD) then

    end
end

function WithinMeleeRange()

    if (Distance3D(componentTransform:GetPosition(), target:GetTransform():GetPosition()) <= meleeRange) then
        return true
    end

    return false
end

function WithinRangedRange()

    if (Distance3D(componentTransform:GetPosition(), target:GetTransform():GetPosition()) <= rangedAttackRange) then
        return true
    end

    return false
end

function EventHandler(key, fields)
    if key == "Change_State" then -- fields[1] -> oldState; fields[2] -> newState;
        currentState = fields[2]
    elseif key == "Target_Update" then
        target = fields[1] -- fields[1] -> new Target;
    elseif key == "Die" then
        if (fields[1] == gameObject) then
            Die()
        end
    elseif key == "Knife_Hit" then
        if (fields[1] == gameObject) then
            if (currentState == STATE.UNAWARE or currentState == STATE.AWARE) then
                knifeHitChance =
                    GetVariable("Zhib.lua", "unawareChanceSardKnife", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
                math.randomseed(os.time())
                rng = math.random(100)
                if (rng <= knifeHitChance) then
                    Log("Knife's D100 roll has been " .. rng .. " so the UNAWARE enemy is dead! \n")
                    Die()
                else
                    Log("Knife's D100 roll has been " .. rng .. " so the UNAWARE enemy has dodged the knife :( \n")
                end
            elseif (currentState == STATE.SUS) then
                knifeHitChance = GetVariable("Zhib.lua", "awareChanceSardKnife", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
                math.randomseed(os.time())
                rng = math.random(100)
                if (rng <= knifeHitChance) then
                    Log("Knife's D100 roll has been " .. rng .. " so the AWARE enemy is dead! \n")
                    Die()
                else
                    Log("Knife's D100 roll has been " .. rng .. " so the AWARE enemy has dodged the knife :( \n")
                end
            elseif (currentState == STATE.AGGRO) then
                knifeHitChance = GetVariable("Zhib.lua", "aggroChanceSardKnife", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
                math.randomseed(os.time())
                rng = math.random(100)
                if (rng <= knifeHitChance) then
                    Log("Knife's D100 roll has been " .. rng .. " so the AGGRO enemy is dead! \n")
                    Die()
                else
                    Log("Knife's D100 roll has been " .. rng .. " so the AGGRO enemy has dodged the knife :( \n")
                end
            end
        end
    elseif key == "Dart_Hit" then
        if (fields[1] == gameObject) then
            if (currentState == STATE.UNAWARE or currentState == STATE.AWARE) then
                dartHitChance =
                    GetVariable("Nerala.lua", "unawareChanceHarkDart", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
                math.randomseed(os.time())
                rng = math.random(100)
                if (rng <= dartHitChance) then
                    Log("Dart's D100 roll has been " .. rng .. " so the UNAWARE enemy is stunned! \n")
                    -- TODO: STUN NOT DIE
                    Die()
                else
                    Log("Dart's D100 roll has been " .. rng .. " so the UNAWARE enemy has dodged the dart :( \n")
                end
            elseif (currentState == STATE.SUS) then
                dartHitChance = GetVariable("Nerala.lua", "awareChanceHarkDart", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
                math.randomseed(os.time())
                rng = math.random(100)
                if (rng <= dartHitChance) then
                    Log("Dart's D100 roll has been " .. rng .. " so the AWARE enemy is stunned! \n")
                    -- TODO: STUN NOT DIE
                    Die()
                else
                    Log("Dart's D100 roll has been " .. rng .. " so the AWARE enemy has dodged the dart :( \n")
                end
            elseif (currentState == STATE.AGGRO) then
                dartHitChance = GetVariable("Nerala.lua", "aggroChanceHarkDart", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
                math.randomseed(os.time())
                rng = math.random(100)
                if (rng <= dartHitChance) then
                    Log("Dart's D100 roll has been " .. rng .. " so the AGGRO enemy is stunned! \n")
                    -- TODO: STUN NOT DIE
                    Die()
                else
                    Log("Dart's D100 roll has been " .. rng .. " so the AGGRO enemy has dodged the dart :( \n")
                end
            end
        end
    end
end

function Die()

    -- Chance to spawn, if spawn dispatch event
    math.randomseed(os.time())
    rng = math.random(100)
    if (rng >= 50) then
        InstantiatePrefab("SpiceLoot")
        str = "Harkonnen"
        DispatchGlobalEvent("Spice_Spawn", {componentTransform:GetPosition(), str})
        Log("Enemy has dropped a spice loot :) " .. rng .. "\n")
    else
        Log("The drop rate has not been good :( " .. rng .. "\n")
    end

    DeleteGameObject()
end

-- Math
function Distance3D(a, b)
    diff = {
        x = b.x - a.x,
        y = b.y - a.y,
        z = b.z - a.z
    }
    return math.sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z)
end

Log("Sardaukar.lua compiled succesfully\n")
print("Sardaukar.lua compiled succesfully")
