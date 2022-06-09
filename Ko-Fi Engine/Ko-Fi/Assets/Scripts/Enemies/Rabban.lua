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
    DispatchEvent("Assign_Type", {"Rabban"})
end

function Update(dt)

end

function EventHandler(key, fields)
    local rng = 0
    if key == "Change_State" then -- fields[1] -> oldState; fields[2] -> newState;
        if fields[1] ~= fields[2] then
            currentState = fields[2]
        end
        -- Player basic attack
    elseif key == "Player_Attack" then
        if (fields[1] == gameObject) then
            DispatchEvent("Missed", {})
        end
        -- Zhib knife
    elseif key == "Knife_Hit" then
        if (fields[1] == gameObject) then
            if (currentState == STATE.UNAWARE or currentState == STATE.AWARE) then
                DispatchEvent("Missed", {})
                trackList = {1}
                ChangeTrack(trackList)
            elseif (currentState == STATE.SUS) then
                DispatchEvent("Missed", {})
                trackList = {1}
                ChangeTrack(trackList)
            elseif (currentState == STATE.AGGRO) then
                DispatchEvent("Missed", {})
                trackList = {1}
                ChangeTrack(trackList)
            end
        end
        -- Zhib Weirding way
    elseif key == "Death_Mark" then
        if (fields[1] == gameObject) then
            DispatchEvent("Missed", {})
        end
        -- Nerala Mosquito
    elseif key == "Mosquito_Hit" then
        if (fields[1] == gameObject) then
            DispatchEvent("Missed", {})
        end
        -- Omozra ñam ñam
    elseif key == "Sadiq_Update_Target" then -- fields[1] -> target; targeted for (1 -> warning; 2 -> eat; 3 -> spit)
        if (fields[1] == gameObject) then
            if (fields[2] == 2) then
                if (currentState == STATE.UNAWARE or currentState == STATE.AWARE) then
                    DispatchEvent("Missed", {})
                elseif (currentState == STATE.SUS) then
                    DispatchEvent("Missed", {})
                elseif (currentState == STATE.AGGRO) then
                    DispatchEvent("Missed", {})
                end
            elseif fields[2] == 4 then
                DispatchEvent("Missed", {})
            end
        end
        -- Nerala dart
    elseif key == "Dart_Hit" then
        if (fields[1] == gameObject) then
            if (currentState == STATE.UNAWARE or currentState == STATE.AWARE) then
                DispatchEvent("Missed", {})
                trackList = {1}
                ChangeTrack(trackList)
            elseif (currentState == STATE.SUS) then
                DispatchEvent("Missed", {})
                trackList = {1}
                ChangeTrack(trackList)
            elseif (currentState == STATE.AGGRO) then
                DispatchEvent("Missed", {})
                trackList = {1}
                ChangeTrack(trackList)
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
