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
    end
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
