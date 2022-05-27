----------------------------------------------------------------------------------------------------------------------------------
--																																--
--		*** Disclamer! ***																										--
--																																--
--		This script has several dependencies to work:																			--
--		1. The game object has to have a rigidBody																				--
--		2. There has to be a GameState.lua script somewhere in the scene in order to receive inputs								--
--		3. The game object has to have a Pathfinder.lua script in order to move													--
--		4. The animator, particles and audioSwitch components will be accessed if they exist but they are not a requirement		--
--																																--
----------------------------------------------------------------------------------------------------------------------------------
--------------------- Player events ---------------------
Movement = {
    IDLE = 1,
    WALK = 2,
    RUN = 3,
    IDLE_CROUCH = 4,
    CROUCH = 5
}
State = {
    IDLE = 1,
    AIM_PRIMARY = 2,
    AIM_SECONDARY = 3,
    AIM_ULTIMATE = 4,
    AIM_ULTIMATE_RECAST = 5,
    WORM = 6,
    DEAD = 7
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
    Disabled = 6,
    Casting = 7 -- Casting is for intern code
}
abilities = {
    AbilityPrimary = AbilityStatus.Normal,
    AbilitySecondary = AbilityStatus.Normal,
    AbilityUltimate = AbilityStatus.Normal,
    AbilityUltimateRecast = AbilityStatus.Normal -- For intern code
}
---------------------------------------------------------

------------------- Variables setter --------------------
target = nil
currentMovement = Movement.IDLE
currentState = State.IDLE
maxHP = 3
currentHP = maxHP
iFrames = 1.5
iFramesTimer = nil

-- Globals --
characterID = 3
speed = 2000
crouchMultiplierPercentage = 66
runMultiplierPercentage = 150
staminaSeconds = 5
recoveryTime = 3
staminaTimer = staminaSeconds
isTired = false

-- Primary ability --
primaryCastRange = 100
primaryCooldown = 5

-- Secondary ability --
maxSpit = 3
secondaryCastRange = 75
secondaryCooldown = 10

-- Ultimate ability --
ultimateCastRange = 75
ultimateCooldown = 30
ultimateSpiceCost = 1000
ultimateRecastRange = 100
---------------------------------------------------------

-------------------- Movement logic ---------------------
doubleClickDuration = 0.5
doubleClickTimer = 0.0
isDoubleClicking = false
isDialogueOpen = false
---------------------------------------------------------

------------------- Inspector setter --------------------
-- -- Globals --
-- maxHPIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
-- maxHPIV = InspectorVariable.new("maxHP", maxHPIVT, maxHP)
-- NewVariable(maxHPIV)

-- speedIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
-- speedIV = InspectorVariable.new("speed", speedIVT, speed)
-- NewVariable(speedIV)

-- crouchMultiplierPercentageIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
-- crouchMultiplierPercentageIV = InspectorVariable.new("crouchMultiplierPercentage", crouchMultiplierPercentageIVT,
--     crouchMultiplierPercentage)
-- NewVariable(crouchMultiplierPercentageIV)

-- runMultiplierPercentageIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
-- runMultiplierPercentageIV = InspectorVariable.new("runMultiplierPercentage", runMultiplierPercentageIVT,
--     runMultiplierPercentage)
-- NewVariable(runMultiplierPercentageIV)

-- staminaSecondsIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
-- staminaSecondsIV = InspectorVariable.new("staminaSeconds", staminaSecondsIVT, staminaSeconds)
-- NewVariable(staminaSecondsIV)

-- recoveryTimeIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
-- recoveryTimeIV = InspectorVariable.new("recoveryTime", recoveryTimeIVT, recoveryTime)
-- NewVariable(recoveryTimeIV)

-- ---- Primary ability --
-- primaryCastRangeIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
-- primaryCastRangeIV = InspectorVariable.new("primaryCastRange", primaryCastRangeIVT, primaryCastRange)
-- NewVariable(primaryCastRangeIV)

-- ---- Secondary ability --
-- secondaryCastRangeIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
-- secondaryCastRangeIV = InspectorVariable.new("secondaryCastRange", secondaryCastRangeIVT, secondaryCastRange)
-- NewVariable(secondaryCastRangeIV)

-- ---- Ultimate ability --
-- ultimateCastRangeIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
-- ultimateCastRangeIV = InspectorVariable.new("ultimateCastRange", ultimateCastRangeIVT, ultimateCastRange)
-- NewVariable(ultimateCastRangeIV)

-- ultimateSpiceCostIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
-- ultimateSpiceCostIV = InspectorVariable.new("ultimateSpiceCost", ultimateSpiceCostIVT, ultimateSpiceCost)
-- NewVariable(ultimateSpiceCostIV)
---------------------------------------------------------

----------------------- Methods -------------------------

function Start()

    -- Components
    componentRigidBody = gameObject:GetRigidBody()
    componentBoxCollider = gameObject:GetBoxCollider()
    componentSwitch = gameObject:GetAudioSwitch()
    componentAnimator = gameObject:GetParent():GetComponentAnimator()
    componentLight = gameObject:GetLight()

    -- Animation
    if (componentAnimator ~= nil) then
        componentAnimator:SetSelectedClip("Idle")
    else
        Log("[ERROR] Component Animator not found!\n")
    end

    -- Particles
    mouseParticles = Find("Mouse Particle")
    if (mouseParticles ~= nil) then
        mouseParticles:GetComponentParticle():StopParticleSpawn()
    end
    choosingTargetParticle = Find("Target Particle")
    bloodParticle = Find("Omozra Blood Particle")
    if (bloodParticle ~= nil) then
        bloodParticle:GetComponentParticle():StopParticleSpawn()
    end
    swooshParticle = Find("Omozra Swoosh Particle") -- not used currently
    footstepsParticle = Find("Omozra Footstep Particle")

    -- Audio
    currentTrackID = -1

    -- HP
    currentHP = maxHP
    DispatchGlobalEvent("Player_Health", {characterID, currentHP, maxHP})

    -- Stamina
    staminaBar = Find("Stamina Bar")
    staminaBarSizeY = staminaBar:GetTransform():GetScale().y

    -- Abilities
    InstantiatePrefab("Worm")
    spitCount = maxSpit
end

-- Called each loop iteration
function Update(dt)

    if (ultimateTimer ~= nil and currentState == State.AIM_ULTIMATE_RECAST) then
        StopMovement(false)
    end
    DrawActiveAbilities()
    DrawHoverParticle()

    if (bloodParticle ~= nil) then
        bloodParticle:GetTransform():SetPosition(float3.new(componentTransform:GetPosition().x,
            componentTransform:GetPosition().y + 23, componentTransform:GetPosition().z + 12))
    end

    DispatchGlobalEvent("Player_Position", {componentTransform:GetPosition(), gameObject})

    if (lastRotation ~= nil) then
        componentTransform:LookAt(lastRotation, float3.new(0, 1, 0))
    end

    if (ManageTimers(dt) == false) then
        return
    end

    -- Manage States
    if (currentState == State.AIM_PRIMARY or currentState == State.AIM_SECONDARY or currentState == State.AIM_ULTIMATE) then
        StopMovement()
        componentAnimator:SetSelectedClip("Idle")
    elseif (destination ~= nil) then
        MoveToDestination(dt)
    end

    -- Gather Inputs
    if (IsSelected() == true) then

        UpdateStaminaBar()

        if (footstepsParticle ~= nil) then
            footstepsParticle:GetTransform():SetPosition(float3.new(componentTransform:GetPosition().x,
                componentTransform:GetPosition().y + 1, componentTransform:GetPosition().z))
        end

        -- Left Click
        if (GetInput(1) == KEY_STATE.KEY_DOWN) then

            -- Primary ability (spit heal)
            if (currentState == State.AIM_PRIMARY) then
                if (spitCount <= 2) then
                    Log("[FAIL] Ability Primary: You don't have enough spits!\n")
                else
                    target = GetGameObjectHovered()
                    if (target.tag ~= Tag.PLAYER) then
                        Log("[FAIL] Ability Primary: You have to select a player first!\n")
                        target = nil
                    else
                        if (Distance3D(target:GetTransform():GetPosition(), componentTransform:GetPosition()) >
                            primaryCastRange) then
                            Log("[FAIL] Ability Primary: Ability out of range!\n")
                            target = nil
                        else
                            if (componentAnimator ~= nil) then
                                CastPrimary(target)
                            end
                        end
                    end
                end

                -- Secondary ability
            elseif (currentState == State.AIM_SECONDARY) then
                if (secondaryTimer ~= nil) then
                    Log("[FAIL] Ability Secondary: Ability in cooldown!\n")
                else
                    target = GetGameObjectHovered()
                    if (target.tag == Tag.ENEMY or target.tag == Tag.CORPSE) then
                        if (Distance3D(target:GetTransform():GetPosition(), componentTransform:GetPosition()) >
                            secondaryCastRange) then
                            Log("[FAIL] Ability Secondary: Ability out of range!\n")
                            target = nil
                        else
                            if (componentAnimator ~= nil) then
                                CastSecondary(target:GetTransform():GetPosition())
                            end
                        end
                    else
                        Log("[FAIL] Ability Secondary: You have to select an enemy or a corpse first!\n")
                        target = nil
                    end
                end

                -- Ultimate ability (needs to be refactored a bit :D)
            elseif (ultimateTimer == nil and currentState == State.AIM_ULTIMATE) then
                target = GetGameObjectHovered()
                if (target.tag ~= Tag.PLAYER) then
                    Log("[FAIL] Ability Ultimate: You have to select an ally first!\n")
                    target = nil
                else
                    if (Distance3D(target:GetTransform():GetPosition(), componentTransform:GetPosition()) >
                        ultimateCastRange) then
                        Log("[FAIL] Ability Ultimate: Ability out of range!\n")
                        target = nil
                    else
                        if (componentAnimator ~= nil and target ~= gameObject) then
                            CastUltimate(target:GetTransform():GetPosition())
                        end
                    end
                end
            elseif (currentState == State.AIM_ULTIMATE_RECAST) then
                target = GetGameObjectHovered() -- This is for the ability to go to the mouse Pos (it uses the target var)
                local mouse = GetLastMouseClick()
                if (Distance3D(mouse, componentTransform:GetPosition()) > ultimateRecastRange) then
                    Log("[FAIL] Ability Ultimate Recast: Ability out of range!\n")
                    target = nil
                else
                    if (target.tag ~= Tag.FLOOR) then
                        Log("[FAIL] Ability Ultimate: You have to select floor!\n")
                        target = nil
                    else
                        target = mouse
                        if (componentAnimator ~= nil) then
                            RecastUltimate(mouse) -- Ult step 6
                        end
                    end
                end
            end
        end

        -- Right Click
        if (GetInput(3) == KEY_STATE.KEY_DOWN) then
            goHit = GetGameObjectHovered()
            if (goHit ~= gameObject) then -- Check you are not right-clicking yourself
                if (currentState == State.AIM_PRIMARY or currentState == State.AIM_SECONDARY or currentState ==
                    State.AIM_ULTIMATE) then
                    CancelAbilities()
                else
                    isMoving = true

                    if (goHit.tag == Tag.PICKUP or goHit.tag == Tag.ENEMY) then
                        Log("Going to a pickup\n")
                        target = nil
                        if (currentState ~= State.AIM_ULTIMATE_RECAST) then
                            SetState(State.IDLE)
                        end
                        destination = goHit:GetTransform():GetPosition()
                        DispatchEvent("Pathfinder_UpdatePath", {{destination}, false, componentTransform:GetPosition()})
                    elseif (goHit.tag == Tag.FLOOR) then
                        target = nil
                        if (currentState ~= State.AIM_ULTIMATE_RECAST) then
                            SetState(State.IDLE)
                        end
                        destination = GetLastMouseClick()
                        DispatchEvent("Pathfinder_UpdatePath", {{destination}, false, componentTransform:GetPosition()})
                    else
                        Log("No possible path\n")
                        target = nil
                        destination = nil
                        if (currentState ~= State.AIM_ULTIMATE_RECAST) then
                            SetState(State.IDLE)
                        end
                        isMoving = false
                        DispatchEvent("Pathfinder_UpdatePath", {{destination}, false, componentTransform:GetPosition()})
                    end

                    if (currentMovement == Movement.WALK and isDoubleClicking == true and isMoving == true and isTired ==
                        false) then
                        SetMovement(Movement.RUN)
                    else
                        if (currentMovement == Movement.IDLE and isMoving == true) then
                            SetMovement(Movement.WALK)
                        end
                        isDoubleClicking = true
                    end

                    if (footstepsParticle ~= nil and destination ~= nil) then
                        footstepsParticle:GetComponentParticle():ResumeParticleSpawn()
                    end

                    if (mouseParticles ~= nil and destination ~= nil) then
                        mouseParticles:GetComponentParticle():SetLoop(true)
                        mouseParticles:GetComponentParticle():ResumeParticleSpawn()
                        mouseParticles:GetTransform():SetPosition(destination)
                    end
                end
            end
        end

        -- 1
        if (GetInput(21) == KEY_STATE.KEY_DOWN and currentState ~= State.AIM_ULTIMATE_RECAST) then
            ActivePrimary()
        end

        -- 2
        if (GetInput(22) == KEY_STATE.KEY_DOWN and currentState ~= State.AIM_ULTIMATE_RECAST) then
            ActiveSecondary()
        end

        -- 3
        if (GetInput(23) == KEY_STATE.KEY_DOWN and currentState ~= State.AIM_ULTIMATE_RECAST) then
            ActiveUltimate()
        end

        -- LSHIFT -> Toggle crouch
        if (GetInput(12) == KEY_STATE.KEY_DOWN) then

            if (currentMovement == Movement.CROUCH) then
                SetMovement(Movement.WALK)
            elseif (currentMovement == Movement.WALK or currentMovement == Movement.RUN) then
                SetMovement(Movement.CROUCH)
            elseif (destination == nil) then
                if (currentMovement == Movement.IDLE) then
                    SetMovement(Movement.IDLE_CROUCH)
                elseif (currentMovement == Movement.IDLE_CROUCH) then
                    SetMovement(Movement.IDLE)
                    if (componentAnimator ~= nil) then
                        componentAnimator:SetSelectedClip("Idle")
                    end
                end
            end
        end
    else
        -- CancelAbilities()
    end
end

--------------------------------------------------

------------------- Functions --------------------
function SetState(newState)
    if (newState == State.IDLE) then
        currentState = State.IDLE
    elseif (newState == State.AIM_PRIMARY) then
        currentState = State.AIM_PRIMARY
        StopMovement(false)
    elseif (newState == State.AIM_SECONDARY) then
        currentState = State.AIM_SECONDARY
        StopMovement(false)
    elseif (newState == State.AIM_ULTIMATE) then
        currentState = State.AIM_ULTIMATE
        StopMovement(false)
    elseif (newState == State.AIM_ULTIMATE_RECAST) then
        currentState = State.AIM_ULTIMATE_RECAST
        StopMovement(false)
    elseif (newState == State.DEAD) then
        currentState = State.DEAD
        StopMovement()
    elseif (newState == State.WORM) then
        currentState = State.WORM
        StopMovement()
    end
end

function SetMovement(newMovement)
    if (newMovement == Movement.IDLE) then
        currentMovement = Movement.IDLE
        if (currentTrackID ~= -1) then
            componentSwitch:StopTrack(currentTrackID)
            currentTrackID = -1
        end
    elseif (newMovement == Movement.WALK) then
        currentMovement = Movement.WALK
        if (componentAnimator ~= nil) then
            componentAnimator:SetSelectedClip("Walk")
        end
        ChangeTrack(0)
    elseif (newMovement == Movement.RUN) then
        currentMovement = Movement.RUN
        if (componentAnimator ~= nil) then
            componentAnimator:SetSelectedClip("Run")
        end
        ChangeTrack(1)
    elseif (newMovement == Movement.IDLE_CROUCH) then
        currentMovement = Movement.IDLE_CROUCH
        if (componentAnimator ~= nil) then
            componentAnimator:SetSelectedClip("IdleCrouch")
        end
        if (componentSwitch ~= nil) then
            if (currentTrackID ~= -1) then
                componentSwitch:StopTrack(currentTrackID)
                currentTrackID = -1
            end
        end
    elseif (newMovement == Movement.CROUCH) then
        currentMovement = Movement.CROUCH
        ChangeTrack(0)
        if (componentAnimator ~= nil) then
            componentAnimator:SetSelectedClip("Crouch")
        end
    end
end

function CancelAbilities(onlyAbilities)
    if (currentState == State.AIM_PRIMARY and abilities.AbilityPrimary == AbilityStatus.Active) then
        abilities.AbilityPrimary = AbilityStatus.Normal
        DispatchGlobalEvent("Player_Ability", {characterID, Ability.Primary, AbilityStatus.Normal})
    elseif (currentState == State.AIM_SECONDARY and abilities.AbilitySecondary == AbilityStatus.Active) then
        abilities.AbilitySecondary = AbilityStatus.Normal
        DispatchGlobalEvent("Player_Ability", {characterID, Ability.Secondary, AbilityStatus.Normal})
    elseif (currentState == State.AIM_ULTIMATE and abilities.AbilityUltimate == AbilityStatus.Active) then
        abilities.AbilityUltimate = AbilityStatus.Normal
        DispatchGlobalEvent("Player_Ability", {characterID, Ability.Ultimate, AbilityStatus.Normal})
    elseif (currentState == State.AIM_ULTIMATE_RECAST and abilities.AbilityUltimateRecast == AbilityStatus.Active) then
        abilities.AbilityUltimateRecast = AbilityStatus.Active
    end

    if (onlyAbilities == nil) then
        if (currentState ~= State.AIM_ULTIMATE_RECAST) then
            SetState(State.IDLE)
        end
    end
end

function DrawHoverParticle()
    if (choosingTargetParticle == nil) then
        return
    end

    if (IsSelected() == true) then
        local drawingTarget = GetGameObjectHovered()
        local finalPosition
        if ((currentState == State.AIM_PRIMARY or currentState == State.AIM_SECONDARY or currentState ==
            State.AIM_ULTIMATE or currentState == State.AIM_ULTIMATE_RECAST) and target ~= nil) then

            if (target.x == nil) then
                t = target:GetTransform():GetPosition()
            else
                t = target
            end
            choosingTargetParticle:GetComponentParticle():SetColor(255, 0, 255, 255)
            finalPosition = float3.new(t.x, t.y + 1, t.z)
        elseif ((currentState == State.AIM_SECONDARY and
            (drawingTarget.tag == Tag.ENEMY or drawingTarget.tag == Tag.CORPSE)) or
            (currentState == State.AIM_ULTIMATE and drawingTarget.tag == Tag.PLAYER)) then
            local dist = Distance3D(drawingTarget:GetTransform():GetPosition(), componentTransform:GetPosition())
            if ((currentState == State.AIM_SECONDARY and dist <= secondaryCastRange) or
                (currentState == State.AIM_ULTIMATE and dist <= ultimateCastRange)) then
                choosingTargetParticle:GetComponentParticle():SetColor(0, 255, 0, 255)
            else
                choosingTargetParticle:GetComponentParticle():SetColor(255, 0, 0, 255)
            end
            finalPosition = drawingTarget:GetTransform():GetPosition()
            finalPosition.y = finalPosition.y + 1
        elseif (currentState == State.AIM_PRIMARY and drawingTarget.tag == Tag.PLAYER) then
            local dist = Distance3D(drawingTarget:GetTransform():GetPosition(), componentTransform:GetPosition())
            if (currentState == State.AIM_PRIMARY and dist <= primaryCastRange) then
                choosingTargetParticle:GetComponentParticle():SetColor(0, 255, 0, 255)
            else
                choosingTargetParticle:GetComponentParticle():SetColor(255, 0, 0, 255)
            end
            finalPosition = drawingTarget:GetTransform():GetPosition()
            finalPosition.y = finalPosition.y + 1
        elseif (currentState == State.AIM_ULTIMATE_RECAST and drawingTarget.tag == Tag.FLOOR) then
            local mouseClick = GetLastMouseClick()
            if (Distance3D(mouseClick, componentTransform:GetPosition()) <= ultimateRecastRange) then
                choosingTargetParticle:GetComponentParticle():SetColor(0, 255, 0, 255)
            else
                choosingTargetParticle:GetComponentParticle():SetColor(255, 0, 0, 255)
            end
            finalPosition = float3.new(mouseClick.x, mouseClick.y + 1, mouseClick.z)
        else
            choosingTargetParticle:GetComponentParticle():StopParticleSpawn()
            return
        end
        choosingTargetParticle:GetComponentParticle():ResumeParticleSpawn()
        choosingTargetParticle:GetTransform():SetPosition(finalPosition)
    end
end

function DrawActiveAbilities()
    if componentLight == nil then
        componentLight = gameObject:GetLight()
    end
    if componentLight ~= nil then
        if (IsSelected() == true) then
            if (abilities.AbilityPrimary == AbilityStatus.Active) then
                componentLight:SetRange(primaryCastRange)
                componentLight:SetAngle(360 / 2)
                componentLight:SetDiffuse(0.5)
            elseif (abilities.AbilitySecondary == AbilityStatus.Active) then
                componentLight:SetRange(secondaryCastRange)
                componentLight:SetAngle(360 / 2)
                componentLight:SetDiffuse(0.5)
            elseif (abilities.AbilityUltimate == AbilityStatus.Active) then
                componentLight:SetRange(ultimateCastRange)
                componentLight:SetAngle(360 / 2)
                componentLight:SetDiffuse(0.5)
            elseif (abilities.AbilityUltimateRecast == AbilityStatus.Active) then
                componentLight:SetRange(ultimateRecastRange)
                componentLight:SetAngle(360 / 2)
                componentLight:SetDiffuse(0.5)
            else
                componentLight:SetAngle(0)
            end
        else
            componentLight:SetAngle(0)
        end
    end
end

function UpdateStaminaBar()
    staminaBar:GetTransform():SetScale(float3.new(staminaBar:GetTransform():GetScale().x,
        staminaBarSizeY * (staminaTimer / staminaSeconds), staminaBar:GetTransform():GetScale().z))
end

function ManageTimers(dt)
    local ret = true

    if (isDialogueOpen == true) then
        ret = false
    end
    if (currentMovement == Movement.RUN and
        GetVariable("GameState.lua", "GodMode", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == false) then
        staminaTimer = staminaTimer - dt
        if (staminaTimer < 0.0) then
            staminaTimer = 0.0
            isTired = true

            SetMovement(Movement.WALK)
        else
            -- Log("Stamina timer: " .. staminaTimer .. "\n")
        end
    else
        staminaTimer = staminaTimer + dt
        if (staminaTimer > recoveryTime) then
            staminaTimer = staminaSeconds
            isTired = false
            -- Log("I am recovered! :) \n")
        else
            -- Log("Stamina timer: " .. staminaTimer .. "\n")
        end
    end

    -- Running state logic
    if (isDoubleClicking == true) then
        if (doubleClickTimer < doubleClickDuration) then
            doubleClickTimer = doubleClickTimer + dt
        else
            isDoubleClicking = false
            doubleClickTimer = 0.0
        end
    end

    -- Invencibility timer
    if (iFramesTimer ~= nil) then
        iFramesTimer = iFramesTimer + dt
        if (iFramesTimer >= iFrames) then
            iFramesTimer = nil
            bloodParticle:GetComponentParticle():StopParticleSpawn()
        end
    end

    -- Primary ability cooldown
    if (spitCount > 2 and
        not (abilities.AbilityPrimary == AbilityStatus.Active or abilities.AbilityPrimary == AbilityStatus.Casting)) then
        abilities.AbilityPrimary = AbilityStatus.Normal
        DispatchGlobalEvent("Player_Ability", {characterID, Ability.Primary, abilities.AbilityPrimary})
    end
    if (currentState == State.AIM_PRIMARY) then
        DispatchGlobalEvent("Omozra_Primary", {})
    end

    -- Secondary ability cooldown
    if (secondaryTimer ~= nil) then
        secondaryTimer = secondaryTimer + dt
        if (secondaryTimer >= secondaryCooldown) then
            secondaryTimer = nil
            abilities.AbilitySecondary = AbilityStatus.Normal
            DispatchGlobalEvent("Player_Ability", {characterID, Ability.Secondary, abilities.AbilitySecondary})
        end
    end

    -- Ultimate ability cooldown
    if (ultimateTimer ~= nil) then
        ultimateTimer = ultimateTimer + dt
        if (ultimateTimer >= ultimateCooldown) then
            ultimateTimer = nil
            abilities.AbilityUltimate = AbilityStatus.Normal
            DispatchGlobalEvent("Player_Ability", {characterID, Ability.Ultimate, abilities.AbilityUltimate})
        end
    end
    if (currentState == State.AIM_ULTIMATE) then
        DispatchGlobalEvent("Omozra_Ultimate", {})
    end

    -- Animation timer
    if (componentAnimator ~= nil) then
        if (componentAnimator:IsCurrentClipLooping() == false) then
            if (componentAnimator:IsCurrentClipPlaying() == true) then
                ret = false
            else
                if (currentState == State.AIM_PRIMARY) then
                    DoPrimary()
                elseif (currentState == State.AIM_SECONDARY) then
                    DoSecondary()
                elseif (currentState == State.AIM_ULTIMATE) then
                    DoUltimate()
                elseif (currentState == State.AIM_ULTIMATE_RECAST) then
                    if (ultimateTimer ~= nil) then -- Second pass will get in here
                        DoUltimateRecast()
                    end
                elseif (currentState ~= State.DEAD) then
                    componentAnimator:SetSelectedClip("Idle") -- Comment this line to test animations in-game
                end
            end
        end
    end

    -- If he's dead he can't do anything
    if (currentState == State.DEAD) then
        ret = false
    end

    return ret
end

function MoveToDestination(dt)
    local targetPos2D = {destination.x, destination.z}
    local pos2D = {componentTransform:GetPosition().x, componentTransform:GetPosition().z}
    local d = Distance2D(pos2D, targetPos2D)
    local vec2 = {targetPos2D[1] - pos2D[1], targetPos2D[2] - pos2D[2]}

    if (d > 5.0) then

        -- Adapt speed
        local s = speed
        if (currentMovement == Movement.IDLE_CROUCH) then
            SetMovement(Movement.CROUCH)
            s = speed * crouchMultiplierPercentage / 100
            DispatchGlobalEvent("Auditory_Trigger", {componentTransform:GetPosition(),
                                                     100 * crouchMultiplierPercentage / 100, "repeated", gameObject})
        elseif (currentMovement == Movement.CROUCH) then
            s = speed * crouchMultiplierPercentage / 100
            DispatchGlobalEvent("Auditory_Trigger", {componentTransform:GetPosition(),
                                                     100 * crouchMultiplierPercentage / 100, "repeated", gameObject})
        elseif (currentMovement == Movement.RUN) then
            s = speed * runMultiplierPercentage / 100
            DispatchGlobalEvent("Auditory_Trigger", {componentTransform:GetPosition(),
                                                     100 * runMultiplierPercentage / 100, "repeated", gameObject})
        elseif (currentMovement == Movement.WALK) then
            DispatchGlobalEvent("Auditory_Trigger", {componentTransform:GetPosition(), 100, "repeated", gameObject})
        end

        -- Adapt speed on arrive
        if (d < 2) then
            s = s * 0.5
        end

        -- Movement
        vec2 = Normalize(vec2, d)
        if (componentRigidBody ~= nil) then
            -- componentRigidBody:SetLinearVelocity(float3.new(vec2[1] * s * dt, 0, vec2[2] * s * dt))
            DispatchEvent("Pathfinder_FollowPath", {s, dt, false})
        end

        -- Rotation
        lastRotation = float3.new(vec2[1], 0, vec2[2])
        componentTransform:LookAt(lastRotation, float3.new(0, 1, 0))
    else
        if (componentAnimator ~= nil) then
            componentAnimator:SetSelectedClip("Idle")
        end
        StopMovement()
    end
end

function StopMovement(resetTarget)

    if (currentMovement == Movement.CROUCH) then
        SetMovement(Movement.IDLE_CROUCH)
    else
        SetMovement(Movement.IDLE)
    end

    destination = nil

    if (resetTarget == nil) then -- Default case
        target = nil
    end
    if (componentRigidBody ~= nil) then
        componentRigidBody:SetLinearVelocity(float3.new(0, 0, 0))
    end
end

function LookAtTarget(lookAt)
    local targetPos2D = {lookAt.x, lookAt.z}
    local pos2D = {componentTransform:GetPosition().x, componentTransform:GetPosition().z}
    local d = Distance2D(pos2D, targetPos2D)
    local vec2 = {targetPos2D[1] - pos2D[1], targetPos2D[2] - pos2D[2]}
    lastRotation = float3.new(vec2[1], 0, vec2[2])
    componentTransform:LookAt(lastRotation, float3.new(0, 1, 0))
end

function IsSelected()

    local id = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)

    if (id == characterID) then
        return true
    end

    return false
end

-- Primary ability
function ActivePrimary()
    if (spitCount > 2) then
        if (currentState == State.AIM_PRIMARY) then
            CancelAbilities()
        else
            CancelAbilities()
            SetState(State.AIM_PRIMARY)
            abilities.AbilityPrimary = AbilityStatus.Active
            DispatchGlobalEvent("Player_Ability", {characterID, Ability.Primary, abilities.AbilityPrimary})
        end
    end
end

function CastPrimary(thisTarget)
    abilities.AbilityPrimary = AbilityStatus.Casting

    componentAnimator:SetSelectedClip("Point")
    StopMovement(false)

    if (thisTarget ~= gameObject) then
        LookAtTarget(thisTarget:GetTransform():GetPosition())
    end
end

function DoPrimary()
    spitCount = spitCount - 3

    if (spitCount > 2) then
        abilities.AbilityPrimary = AbilityStatus.Normal
        DispatchGlobalEvent("Player_Ability", {characterID, Ability.Primary, abilities.AbilityPrimary})
    else
        abilities.AbilityPrimary = AbilityStatus.Cooldown -- Should be state disabled 
        DispatchGlobalEvent("Player_Ability", {characterID, Ability.Primary, abilities.AbilityPrimary})
    end

    DispatchGlobalEvent("Sadiq_Heal", {target, gameObject:GetTransform():GetPosition()})

    componentAnimator:SetSelectedClip("PointToIdle")
    SetState(State.IDLE)

    target = nil
end

-- Secondary ability
function ActiveSecondary()
    if (secondaryTimer == nil) then
        if (currentState == State.AIM_SECONDARY) then
            CancelAbilities()
        else
            CancelAbilities()
            SetState(State.AIM_SECONDARY)
            abilities.AbilitySecondary = AbilityStatus.Active
            DispatchGlobalEvent("Player_Ability", {characterID, Ability.Secondary, abilities.AbilitySecondary})
        end
    end
end

function CastSecondary(position)
    abilities.AbilitySecondary = AbilityStatus.Casting

    componentAnimator:SetSelectedClip("Point")
    StopMovement(false)

    LookAtTarget(position)

end

function DoSecondary()
    spitCount = spitCount + 1

    secondaryTimer = 0.0
    abilities.AbilitySecondary = AbilityStatus.Cooldown
    DispatchGlobalEvent("Player_Ability", {characterID, Ability.Secondary, abilities.AbilitySecondary})

    ChangeTrack(4)

    DispatchGlobalEvent("Sadiq_Update_Target", {target, 1}) -- fields[1] -> target; targeted for (1 -> warning; 2 -> eat; 3 -> spit)

    componentAnimator:SetSelectedClip("PointToIdle")
    SetState(State.IDLE)

    target = nil
end

-- Ultimate ability
function ActiveUltimate()
    if (ultimateTimer == nil and abilities.AbilityUltimate == AbilityStatus.Normal and
        (GetVariable("GameState.lua", "spiceAmount", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) >= ultimateSpiceCost or
            GetVariable("GameState.lua", "GodMode", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true)) then
        if (currentState == State.AIM_ULTIMATE) then
            CancelAbilities()
        else
            CancelAbilities()
            SetState(State.AIM_ULTIMATE)
            abilities.AbilityUltimate = AbilityStatus.Active
            DispatchGlobalEvent("Player_Ability", {characterID, Ability.Ultimate, AbilityStatus.Active})
        end
    end
end

function CastUltimate(position) -- Ult step 3
    if (GetVariable("GameState.lua", "GodMode", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == false) then
        -- Subtracts spice cost when using ultimate ability
        OGSpice = GetVariable("GameState.lua", "spiceAmount", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        NewSpice = OGSpice - ultimateSpiceCost
        SetVariable(NewSpice, "GameState.lua", "spiceAmount", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)

        str = "Spice Amount " .. NewSpice .. "\n"
        Log(str)
    end

    abilities.AbilityUltimate = AbilityStatus.Casting

    componentAnimator:SetSelectedClip("Point")

    StopMovement(false)

    LookAtTarget(target:GetTransform():GetPosition())

    ChangeTrack(4)
end

function DoUltimate() -- Ult step 4
    abilities.AbilityUltimate = AbilityStatus.Using
    DispatchGlobalEvent("Player_Ability", {characterID, Ability.Ultimate, abilities.AbilityUltimate})

    DispatchGlobalEvent("Sadiq_Update_Target", {target, 1}) -- fields[1] -> target; fields[2] -> targeted for (1 -> warning; 2 -> eat; 3 -> spit)
    StopMovement()
    componentAnimator:SetSelectedClip("PointToIdle")

    SetState(State.AIM_ULTIMATE_RECAST)
    abilities.AbilityUltimateRecast = AbilityStatus.Active -- Used this only for drawing
end

function RecastUltimate(position)

    abilities.AbilityUltimateRecast = AbilityStatus.Casting

    componentAnimator:SetSelectedClip("Point")

    StopMovement(false)

    LookAtTarget(position)

    ChangeTrack(4)

end

function DoUltimateRecast() -- Ult step 7
    ultimateTimer = 0.0
    abilities.AbilityUltimate = AbilityStatus.Cooldown
    DispatchGlobalEvent("Player_Ability", {characterID, Ability.Ultimate, AbilityStatus.Cooldown})

    abilities.AbilityUltimateRecast = AbilityStatus.Normal -- Used this only for drawing

    DispatchGlobalEvent("Sadiq_Update_Target", {target, 3}) -- fields[1] -> target; fields[2] -> targeted for (1 -> warning; 2 -> eat; 3 -> spit)
    StopMovement(false)
    componentAnimator:SetSelectedClip("PointToIdle")
    SetState(State.IDLE)
end

function TakeDamage(damage)
    if (iFramesTimer ~= nil or currentHP == 0 or
        GetVariable("GameState.lua", "GodMode", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true) then
        return
    end

    iFramesTimer = 0
    bloodParticle:GetComponentParticle():ResumeParticleSpawn()

    if (damage == nil) then
        damage = 1
    end

    if (currentHP > 1) then
        currentHP = currentHP - damage
        DispatchGlobalEvent("Player_Health", {characterID, currentHP, maxHP})

        ChangeTrack(2)
    else
        currentHP = 0
        DispatchGlobalEvent("Player_Health", {characterID, currentHP, maxHP})
        Die()
    end
end

function Die()
    SetState(State.DEAD)
    currentHP = 0
    DispatchGlobalEvent("Player_Death", {characterID})

    if (componentRigidBody ~= nil) then
        componentRigidBody:SetRigidBodyPos(float3.new(componentTransform:GetPosition().x, 3,
            componentTransform:GetPosition().z))
    end
    if (componentAnimator ~= nil) then
        componentAnimator:SetSelectedClip("Death")
    end
    ChangeTrack(3)
end
--------------------------------------------------

-------------------- Events ----------------------
function EventHandler(key, fields)

    if (key == "Stop_Movement") then
        StopMovement()
        if (componentAnimator ~= nil) then
            componentAnimator:SetSelectedClip("Idle")
        end
    elseif (key == "Enemy_Attack") then
        if (fields[1] == gameObject) then
            if (fields[2] == "Harkonnen") then
                TakeDamage()
            end
        end
    elseif (key == "Active_Primary") then
        if fields[1] == characterID then
            ActivePrimary()
        end
    elseif (key == "Active_Secondary") then
        if fields[1] == characterID then
            ActiveSecondary()
        end
    elseif (key == "Active_Ultimate") then
        if (fields[1] == characterID) then
            ActiveUltimate()
        end
    elseif (key == "Changed_Character") then -- fields[1] -> From ////// fields[2] -> To
        if (fields[1] == characterID) then
            -- If omozra is being changed
            CancelAbilities(true)
        end
        if (fields[2] == characterID) then
            -- If game changed to omozra, update HUD events depending on Abilities
            DispatchGlobalEvent("Player_Ability", {characterID, Ability.Primary, abilities.AbilityPrimary})
            -- Log("Omozra: Primary = " .. abilities.AbilityPrimary .. "\n")
            DispatchGlobalEvent("Player_Ability", {characterID, Ability.Secondary, abilities.AbilitySecondary})
            -- Log("Omozra: Secondary = " .. abilities.AbilitySecondary .. "\n")
            DispatchGlobalEvent("Player_Ability", {characterID, Ability.Ultimate, abilities.AbilityUltimate})
            -- Log("Omozra: Ultimate = " .. abilities.AbilityUltimate .. "\n")
        end
    elseif (key == "Dialogue_Opened") then
        isDialogueOpen = true
        StopMovement()
        SetState(State.IDLE)
        componentAnimator:SetSelectedClip("Idle")
    elseif (key == "Dialogue_Closed") then
        isDialogueOpen = false
    elseif (key == "Spice_Reward") then
        -- ChangeTrack(8)
    elseif (key == "Spit_Heal_Hit") then
        if (fields[1] == gameObject) then
            if (currentHP < maxHP) then
                currentHP = currentHP + 1
                DispatchGlobalEvent("Player_Health", {characterID, currentHP, maxHP})
                Log("Sadiq has healed Omozra. Current HP = " .. currentHP .. "\n")
            else
                Log("Sadiq has healed Omozra, but it was already full HP\n")
            end
        end
    end
end
--------------------------------------------------

------------------ Collisions --------------------
function OnTriggerEnter(go)
    if (go.tag == Tag.ENEMY and iFramesTimer == nil) then
        -- TakeDamage(1)
    end
end

function OnCollisionEnter(go)
    if (currentState == State.DEAD) then
        return
    end
    if (go.tag == Tag.ENEMY and iFramesTimer == nil) then
        -- TakeDamage(1)
    end
end

--------------------------------------------------

----------------- Math Functions -----------------
function Normalize(vec, distance)
    vec[1] = vec[1] / distance
    vec[2] = vec[2] / distance
    return vec
end

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

--------------------------------------------------

function ChangeTrack(index)
    if (componentSwitch ~= nil) then
        if (currentTrackID ~= -1) then
            componentSwitch:StopTrack(currentTrackID)
        end
        currentTrackID = index
        componentSwitch:PlayTrack(currentTrackID)
    end
end

print("Omozra.lua compiled succesfully!\n")
Log("Omozra.lua compiled succesfully!\n")
