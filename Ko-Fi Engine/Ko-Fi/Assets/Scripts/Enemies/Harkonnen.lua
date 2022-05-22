STATE = { -- Importat to add changes to the state enum in EnemyController.lua and add them here
    UNAWARE = 1,
    AWARE = 2,
    SUS = 3,
    AGGRO = 4,
    DEAD = 5,
    VICTORY = 6
}

ATTACK_FASE = {
    IDLE = 1,
    BEGIN_ATTACK = 2,
    DO_ATTACK = 3
}

meleeRange = 25.0
attackTime = 1.5

knifeHitChance = 100
dartHitChance = 100

function Start()
    currentState = STATE.UNAWARE
    currentAttack = nil
    target = nil
    componentAnimator = gameObject:GetParent():GetComponentAnimator()
end

function Update(dt)

    if (ManageTimers(dt) == false) then
        return
    end

    if (currentState == STATE.UNAWARE) then
        componentAnimator:SetSelectedClip("Idle")
    elseif (currentState == STATE.AWARE) then

    elseif (currentState == STATE.SUS) then

    elseif (currentState == STATE.AGGRO) then
        if (target ~= nil) then
            if (WithinMeleeRange() == true) then
                MeleeAttack()
            elseif (target ~= nil) then
                MoveTowardsTarget()
            else
                -- Keep doing whatever it was doing
            end
        end
    elseif (currentState == STATE.DEAD) then

    end
end

function ManageTimers(dt)
    local ret = true

    -- Basic attack cooldown
    if (attackTimer ~= nil) then
        attackTimer = attackTimer + dt
        if (attackTimer >= attackTime) then
            attackTimer = nil
        end
    end

    -- Animation timer
    if (componentAnimator ~= nil) then
        if (componentAnimator:IsCurrentClipLooping() == false) then
            if (componentAnimator:IsCurrentClipPlaying() == true) then
                ret = false
            else
                if (currentAttack == ATTACK_FASE.BEGIN_ATTACK) then
                    DoAttack()
                elseif (currentState ~= STATE.DEAD) then
                    componentAnimator:SetSelectedClip("Idle") -- Comment this line to test animations in-game
                    currentAttack = nil
                end
            end
        end
    end

    -- If he's dead he can't do anything
    if (currentState == STATE.DEAD) then
        ret = false
    end

    return ret
end

function LookAtTarget(lookAt)
    local targetPos2D = {lookAt.x, lookAt.z}
    local pos2D = {componentTransform:GetPosition().x, componentTransform:GetPosition().z}
    local d = Distance2D(pos2D, targetPos2D)
    local vec2 = {targetPos2D[1] - pos2D[1], targetPos2D[2] - pos2D[2]}
    local lastRotation = float3.new(vec2[1], 0, vec2[2])
    componentTransform:LookAt(lastRotation, float3.new(0, 1, 0))
end

-- Basic Attack
function WithinMeleeRange()

    if (Distance3D(componentTransform:GetPosition(), target:GetTransform():GetPosition()) <= meleeRange) then
        return true
    end

    return false
end

function MeleeAttack()

    if (componentAnimator ~= nil) then
        componentAnimator:SetSelectedClip("Attack")
    end
    LookAtTarget(target:GetTransform():GetPosition())
    currentAttack = ATTACK_FASE.BEGIN_ATTACK
end

function DoAttack()

    componentAnimator:SetSelectedClip("AttackToIdle")

    DispatchGlobalEvent("Enemy_Attack", {target, gameObject})

    LookAtTarget(target:GetTransform():GetPosition())
    attackTimer = 0.0
    currentAttack = ATTACK_FASE.DO_ATTACK
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
                    GetVariable("Zhib.lua", "unawareChanceHarkKnife", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
                math.randomseed(os.time())
                rng = math.random(100)
                if (rng <= knifeHitChance) then
                    Log("Knife's D100 roll has been " .. rng .. " so the UNAWARE enemy is dead! \n")
                    Die()
                else
                    Log("Knife's D100 roll has been " .. rng .. " so the UNAWARE enemy has dodged the knife :( \n")
                end
            elseif (currentState == STATE.SUS) then
                knifeHitChance = GetVariable("Zhib.lua", "awareChanceHarkKnife", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
                math.randomseed(os.time())
                rng = math.random(100)
                if (rng <= knifeHitChance) then
                    Log("Knife's D100 roll has been " .. rng .. " so the AWARE enemy is dead! \n")
                    Die()
                else
                    Log("Knife's D100 roll has been " .. rng .. " so the AWARE enemy has dodged the knife :( \n")
                end
            elseif (currentState == STATE.AGGRO) then
                knifeHitChance = GetVariable("Zhib.lua", "aggroChanceHarkKnife", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
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
function Distance2D(a, b)
    local dx, dy = a[1] - b[1], a[2] - b[2]
    local sqrt = math.sqrt(dx * dx + dy * dy)
    return sqrt
end

function Distance3D(a, b)
    diff = {
        x = b.x - a.x,
        y = b.y - a.y,
        z = b.z - a.z
    }
    return math.sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z)
end

Log("Harkonnen.lua compiled succesfully\n")
print("Harkonnen.lua compiled succesfully")
