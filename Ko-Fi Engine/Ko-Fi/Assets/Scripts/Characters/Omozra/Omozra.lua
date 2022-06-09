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
    PASSIVE = 2,
    AIM_PRIMARY = 3,
    AIM_SECONDARY = 4,
    AIM_ULTIMATE = 5,
    AIM_ULTIMATE_RECAST = 6,
    WORM = 7,
    DEAD = 8
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
speed = 1600
crouchMultiplierPercentage = 67
runMultiplierPercentage = 133
staminaSeconds = 6
recoveryTime = 7
staminaTimer = staminaSeconds
standingStaminaMultiplier = 1.5
isTired = false

-- Passive -- 
passiveRange = 100
maxCharges = 6
currentCharges = maxCharges

-- Primary ability --
primaryCastRange = 225
primaryChargeCost = 3
primaryHealAmount = 1

-- Secondary ability --
secondaryCastRange = 225
secondaryCooldown = 2
unawareChanceHarkSecondary = 100
awareChanceHarkSecondary = 100
aggroChanceHarkSecondary = 100
unawareChanceSardSecondary = 100
awareChanceSardSecondary = 100
aggroChanceSardSecondary = 100
secondaryChargeCost = 6

-- Ultimate ability --
ultimateCastRange = 150
ultimateCooldown = 2
ultimateSpiceCost = 1000
ultimateRecastRange = 100
---------------------------------------------------------

-------------------- Movement logic ---------------------
doubleClickDuration = 0.5
doubleClickTimer = 0.0
isDoubleClicking = false
isDialogueOpen = false
hasToMove = false
---------------------------------------------------------

------------------- Detection logic ---------------------
smokebombFlag = false
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
    characterSelectedParticle = Find("Selected Particle")
    mouseParticles = Find("Mouse Particle")
    if (mouseParticles ~= nil) then
        mouseParticles:GetComponentParticle():StopParticleSpawn()
    end
    choosingTargetParticle = Find("Target Particle")
    bloodParticle = Find("Omozra Blood Particle")
    if (bloodParticle ~= nil) then
        bloodParticle:GetComponentParticle():StopParticleSpawn()
    end
    footstepsParticle = Find("Omozra Footstep Particle")

    -- Audio
    currentTrackID = -1

    -- HP
    currentHP = maxHP
    DispatchGlobalEvent("Player_Health", {characterID, currentHP, maxHP})

    -- Abilities
    InstantiatePrefab("Worm")
    DispatchGlobalEvent("Omozra_Charges", {currentCharges, maxCharges})

    -- Stamina Bar Blue
    staminaBar = Find("Stamina Bar Fill")
end

-- Called each loop iteration
function Update(dt)
    isSelected = IsSelected()

    DrawActiveAbilities()
    DrawHoverParticle()

    if (bloodParticle ~= nil) then
        bloodParticle:GetTransform():SetPosition(float3.new(componentTransform:GetPosition().x,
            componentTransform:GetPosition().y + 23, componentTransform:GetPosition().z + 12))
    end

    if GetVariable("GameState.lua", "omozraAvailable", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true then
        if (smokebombPosition == nil) then
            DispatchGlobalEvent("Player_Position", {componentTransform:GetPosition(), gameObject})
        elseif (Distance3D(componentTransform:GetPosition(), smokebombPosition) > smokebombRadius) then
            DispatchGlobalEvent("Player_Position", {componentTransform:GetPosition(), gameObject})
        end
    end

    if (lastRotation ~= nil) then
        componentTransform:LookAt(lastRotation, float3.new(0, 1, 0))
    end

    if (ManageTimers(dt) == false) then
        do
            return
        end
    end

    -- States
    if (currentMovement ~= Movement.IDLE and currentMovement ~= Movement.IDLE_CROUCH and target ~= nil) then
        if (currentState == State.PASSIVE) then
            if (math.abs(Distance3D(componentTransform:GetPosition(), target:GetTransform():GetPosition())) <=
                passiveRange) then
                Passive()
            else
                hasToMove = true
            end
        elseif (currentState == State.AIM_PRIMARY) then
            if (math.abs(Distance3D(componentTransform:GetPosition(), target:GetTransform():GetPosition())) <=
                primaryCastRange) then
                if (componentAnimator ~= nil) then
                    CastPrimary()
                end
            else
                if (math.abs(Distance3D(destination, target:GetTransform():GetPosition())) >= 5) then
                    destination = target:GetTransform():GetPosition()
                    DispatchEvent("Pathfinder_UpdatePath", {{destination}, false, componentTransform:GetPosition()})
                end
                hasToMove = true
            end
        elseif (currentState == State.AIM_SECONDARY) then
            if (math.abs(Distance3D(componentTransform:GetPosition(), target:GetTransform():GetPosition())) <=
                secondaryCastRange) then
                if (componentAnimator ~= nil) then
                    CastSecondary()
                end
            else
                if (math.abs(Distance3D(destination, target:GetTransform():GetPosition())) >= 5) then
                    destination = target:GetTransform():GetPosition()
                    DispatchEvent("Pathfinder_UpdatePath", {{destination}, false, componentTransform:GetPosition()})
                end
                hasToMove = true
            end
        elseif (currentState == State.AIM_ULTIMATE) then
            if (math.abs(Distance3D(componentTransform:GetPosition(), target:GetTransform():GetPosition())) <=
                ultimateCastRange) then
                if (componentAnimator ~= nil) then
                    CastUltimate()
                end
            else
                if (math.abs(Distance3D(destination, target:GetTransform():GetPosition())) >= 5) then
                    destination = target:GetTransform():GetPosition()
                    DispatchEvent("Pathfinder_UpdatePath", {{destination}, false, componentTransform:GetPosition()})
                end
                hasToMove = true
            end
        elseif currentState == State.AIM_ULTIMATE_RECAST then
            if (math.abs(Distance3D(componentTransform:GetPosition(), target)) <= ultimateRecastRange) then
                if (componentAnimator ~= nil) then
                    RecastUltimate(true)
                end
            else
                hasToMove = true
            end
        end
    elseif (currentState == State.AIM_PRIMARY or currentState == State.AIM_SECONDARY or currentState ==
        State.AIM_ULTIMATE) then
        StopMovement()
        if (currentMovement == Movement.IDLE_CROUCH) then
            componentAnimator:SetSelectedClip("IdleCrouch")
        else
            componentAnimator:SetSelectedClip("Idle")
        end
    elseif (destination ~= nil) then
        hasToMove = true
    end

    -- Gather Inputs
    if (isSelected == true) then

        UpdateStamina()

        -- Left Click
        if (GetInput(1) == KEY_STATE.KEY_DOWN) then

            -- Primary ability (spit heal)
            if (currentState == State.AIM_PRIMARY) then
                CastPrimary()

                -- Secondary ability
            elseif (currentState == State.AIM_SECONDARY) then
                CastSecondary()

                -- Ultimate ability (needs to be refactored a bit :D)
            elseif (ultimateTimer == nil and currentState == State.AIM_ULTIMATE) then
                CastUltimate()
            elseif (currentState == State.AIM_ULTIMATE_RECAST) then
                RecastUltimate(false)
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

                    if (goHit.tag == Tag.CORPSE) then
                        SetState(State.PASSIVE)
                        target = goHit
                        if (Distance3D(componentTransform:GetPosition(), goHit:GetTransform():GetPosition()) <=
                            passiveRange) then
                            isMoving = false
                            Passive()
                        else
                            if (footstepsParticle ~= nil) then
                                feetTimer = 0.5
                                FootstepMovement()
                            end
                            destination = target:GetTransform():GetPosition()
                            if (currentMovement == Movement.IDLE and isMoving == true) then
                                SetMovement(Movement.WALK)
                            end
                            DispatchEvent("Pathfinder_UpdatePath",
                                {{destination}, false, componentTransform:GetPosition()})
                        end
                    elseif (goHit.tag == Tag.PICKUP or goHit.tag == Tag.ENEMY) then
                        target = nil
                        if (currentState ~= State.AIM_ULTIMATE_RECAST) then
                            SetState(State.IDLE)
                        end
                        if (footstepsParticle ~= nil) then
                            feetTimer = 0.5
                            FootstepMovement()
                        end
                        destination = goHit:GetTransform():GetPosition()
                        DispatchEvent("Pathfinder_UpdatePath", {{destination}, false, componentTransform:GetPosition()})
                    elseif (goHit.tag == Tag.FLOOR) then
                        target = nil
                        if (currentState ~= State.AIM_ULTIMATE_RECAST) then
                            SetState(State.IDLE)
                        end
                        if (footstepsParticle ~= nil) then
                            feetTimer = 0.5
                            FootstepMovement()
                        end
                        destination = GetLastMouseClick()
                        DispatchEvent("Pathfinder_UpdatePath", {{destination}, false, componentTransform:GetPosition()})
                    else
                        Log("No possible path\n")
                        target = nil
                        if (footstepsParticle ~= nil) then
                            footstepsParticle:GetComponentParticle():StopParticleSpawn()
                        end
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
                        feetTimer = 0.5
                        FootstepMovement()
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

    if (hasToMove == true and destination ~= nil) then
        MoveToDestination(dt)
        hasToMove = false
    end

    if abilities.AbilityPrimary == AbilityStatus.Using then
        isUsingQ = true
    else
        isUsingQ = false
    end
    if abilities.AbilitySecondary == AbilityStatus.Using then
        isUsingW = true
    else
        isUsingW = false
    end
    if abilities.AbilityUltimate == AbilityStatus.Using then
        isUsingE = true
    else
        isUsingE = false
    end
end

--------------------------------------------------

------------------- Functions --------------------
function SetState(newState)
    if (newState == State.IDLE) then
        currentState = State.IDLE
    elseif (newState == State.PASSIVE) then
        currentState = State.PASSIVE
        StopMovement(false)
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
        trackList = {9}
        ChangeTrack(trackList)
    elseif (newMovement == Movement.RUN) then
        currentMovement = Movement.RUN
        if (componentAnimator ~= nil) then
            componentAnimator:SetSelectedClip("Run")
        end
        trackList = {10}
        ChangeTrack(trackList)
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
        trackList = {9}
        ChangeTrack(trackList)
        if (componentAnimator ~= nil) then
            componentAnimator:SetSelectedClip("Crouch")
        end
    end
end

function CancelAbilities(onlyAbilities)
    if (currentState == State.AIM_PRIMARY and abilities.AbilityPrimary == AbilityStatus.Active) then
        abilities.AbilityPrimary = AbilityStatus.Normal
        DispatchGlobalEvent("Player_Ability", {characterID, Ability.Primary, abilities.AbilityPrimary})
    elseif (currentState == State.AIM_SECONDARY and abilities.AbilitySecondary == AbilityStatus.Active) then
        abilities.AbilitySecondary = AbilityStatus.Normal
        DispatchGlobalEvent("Player_Ability", {characterID, Ability.Secondary, abilities.AbilitySecondary})
    elseif (currentState == State.AIM_ULTIMATE and abilities.AbilityUltimate == AbilityStatus.Active) then
        abilities.AbilityUltimate = AbilityStatus.Normal
        DispatchGlobalEvent("Player_Ability", {characterID, Ability.Ultimate, abilities.AbilityUltimate})
    elseif (currentState == State.AIM_ULTIMATE_RECAST and abilities.AbilityUltimateRecast == AbilityStatus.Active) then
        abilities.AbilityUltimateRecast = AbilityStatus.Active
    end

    if (onlyAbilities == nil) then
        if componentAnimator ~= nil then
            if (currentMovement == Movement.IDLE_CROUCH) then
                componentAnimator:SetSelectedClip("IdleCrouch")
            else
                componentAnimator:SetSelectedClip("Idle")
            end
        end

        if (currentState ~= State.AIM_ULTIMATE_RECAST) then
            SetState(State.IDLE)
        end
        StopMovement()
    end
end

function DrawHoverParticle()
    if (choosingTargetParticle == nil) then
        do
            return
        end
    end

    if (isSelected == true) then
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
            (currentState == State.AIM_ULTIMATE and drawingTarget.tag == Tag.PLAYER and drawingTarget ~= gameObject)) then
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
            do
                return
            end
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
        if (isSelected == true) then
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

function UpdateStamina()
    local proportion = staminaTimer / staminaSeconds
    local recoveryProportion = staminaTimer / recoveryTime

    if proportion >= 0.5 then -- From Green to Yellow
        characterSelectedParticle:GetComponentParticle():SetColor((2 - (proportion * 2)) * 255, 255, 0, 255)
    else -- From Yellow to Red
        characterSelectedParticle:GetComponentParticle():SetColor(255, (proportion * 2) * 255, 0, 255)
    end

    if staminaBar ~= nil then
        staminaBar:GetTransform2D():SetMask(float2.new(proportion, 1))
    end
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
        if (currentMovement == Movement.IDLE or currentMovement == Movement.IDLE_CROUCH) then
            staminaTimer = staminaTimer + dt * standingStaminaMultiplier
        else
            staminaTimer = staminaTimer + dt
        end
        if staminaTimer / staminaSeconds >= 1 then
            staminaTimer = staminaSeconds
            isTired = false
        end
        -- Log("Stamina timer: " .. staminaTimer .. "\n")
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
                if (currentState == State.PASSIVE) then
                    DoPassive()
                elseif (currentState == State.AIM_PRIMARY) then
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
                    if (currentMovement == Movement.IDLE_CROUCH) then
                        componentAnimator:SetSelectedClip("IdleCrouch")
                    else
                        componentAnimator:SetSelectedClip("Idle")
                    end
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

feetTimer = 0.0
leftFoot = true
function FootstepMovement()
    if (footstepsParticle ~= nil) then
        if (feetTimer > 0.5) then
            feetTimer = 0.0
            leftFoot = not leftFoot
            footstepsParticle:GetComponentParticle():ResumeParticleSpawn()
        end

        if (leftFoot == true) then
            footstepsParticle:GetTransform():SetPosition(float3.new(componentTransform:GetPosition().x - 2,
                componentTransform:GetPosition().y + 1, componentTransform:GetPosition().z))
        else
            footstepsParticle:GetTransform():SetPosition(float3.new(componentTransform:GetPosition().x + 2,
                componentTransform:GetPosition().y + 1, componentTransform:GetPosition().z))
        end

    end
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

        if (footstepsParticle ~= nil) then
            feetTimer = feetTimer + dt
            FootstepMovement()
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
    elseif (currentMovement ~= Movement.IDLE_CROUCH) then
        SetMovement(Movement.IDLE)
    end

    destination = nil

    if (resetTarget == nil) then -- Default case
        target = nil
    end
    if (componentRigidBody ~= nil) then
        componentRigidBody:SetLinearVelocity(float3.new(0, 0, 0))
    end
    if (footstepsParticle ~= nil) then
        footstepsParticle:GetComponentParticle():StopParticleSpawn()
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

-- Passive
function Passive()

    SetState(State.PASSIVE)
    componentAnimator:SetSelectedClip("Point")
    LookAtTarget(target:GetTransform():GetPosition())

end

function DoPassive()

    currentCharges = currentCharges + 1
    DispatchGlobalEvent("Omozra_Charges", {currentCharges, maxCharges})

    DispatchGlobalEvent("Sadiq_Update_Target", {target, 1}) -- fields[1] -> target; targeted for (1 -> warning; 2 -> eat; 3 -> spit)

    componentAnimator:SetSelectedClip("PointToIdle")
    SetState(State.IDLE)

    target = nil
end

-- Primary ability
function ActivePrimary()
    if (currentCharges >= primaryChargeCost) then
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

function CastPrimary()
    if (primaryTimer ~= nil) then
        Log("[FAIL] Ability Primary: Ability in cooldown!\n")
        do
            return
        end
    else
        if (target == nil) then
            target = GetGameObjectHovered()
        end
        if (target.tag ~= Tag.PLAYER) then
            Log("[FAIL] Ability Primary: You have to select an enemy first!\n")
            target = nil
            do
                return
            end
        else
            if (math.abs(Distance3D(target:GetTransform():GetPosition(), componentTransform:GetPosition())) <=
                primaryCastRange) then
                if (componentAnimator ~= nil) then
                    abilities.AbilityPrimary = AbilityStatus.Using
                    DispatchGlobalEvent("Player_Ability", {characterID, Ability.Primary, abilities.AbilityPrimary})

                    componentAnimator:SetSelectedClip("Point")
                    StopMovement(false)

                    trackList = {4, 5, 6}
                    ChangeTrack(trackList)

                    if (target ~= gameObject) then
                        LookAtTarget(target:GetTransform():GetPosition())
                    end
                end
            else
                if (footstepsParticle ~= nil) then
                    feetTimer = 0.5
                    FootstepMovement()
                end
                destination = target:GetTransform():GetPosition()
                SetMovement(Movement.WALK)
                DispatchEvent("Pathfinder_UpdatePath", {{destination}, false, componentTransform:GetPosition()})
            end
        end
    end
end

function DoPrimary()
    currentCharges = currentCharges - primaryChargeCost
    DispatchGlobalEvent("Omozra_Charges", {currentCharges, maxCharges})

    DispatchGlobalEvent("Sadiq_Heal", {target, componentTransform:GetPosition(), primaryHealAmount}) -- fields[1] -> target; fields[2] -> pos;

    componentAnimator:SetSelectedClip("PointToIdle")
    SetState(State.IDLE)

    target = nil
end

-- Secondary ability
function ActiveSecondary()
    if (secondaryTimer == nil and currentCharges >= secondaryChargeCost) then
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

function CastSecondary()
    if (secondaryTimer ~= nil) then
        Log("[FAIL] Ability Secondary: Ability in cooldown!\n")
        do
            return
        end
    else
        if (target == nil) then
            target = GetGameObjectHovered()
        end
        if (target.tag ~= Tag.ENEMY and target.tag ~= Tag.CORPSE) then
            Log("[FAIL] Ability Secondary: You have to select an enemy first!\n")
            target = nil
            do
                return
            end
        end
        if (math.abs(Distance3D(target:GetTransform():GetPosition(), componentTransform:GetPosition())) <=
            secondaryCastRange) then
            if (componentAnimator ~= nil) then
                abilities.AbilitySecondary = AbilityStatus.Using
                DispatchGlobalEvent("Player_Ability", {characterID, Ability.Secondary, abilities.AbilitySecondary})

                componentAnimator:SetSelectedClip("Point")
                StopMovement(false)

                trackList = {4, 5, 6}
                ChangeTrack(trackList)

                LookAtTarget(target:GetTransform():GetPosition())
            end
        else
            if (footstepsParticle ~= nil) then
                feetTimer = 0.5
                FootstepMovement()
            end
            destination = target:GetTransform():GetPosition()
            SetMovement(Movement.WALK)
            DispatchEvent("Pathfinder_UpdatePath", {{destination}, false, componentTransform:GetPosition()})
        end
    end
end

function DoSecondary()
    currentCharges = currentCharges - secondaryChargeCost
    DispatchGlobalEvent("Omozra_Charges", {currentCharges, maxCharges})

    secondaryTimer = 0.0
    -- abilities.AbilitySecondary = AbilityStatus.Cooldown
    -- DispatchGlobalEvent("Player_Ability",
    --     {characterID, Ability.Secondary, abilities.AbilitySecondary, secondaryCooldown})

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
            DispatchGlobalEvent("Player_Ability", {characterID, Ability.Ultimate, abilities.AbilityUltimate})
        end
    elseif (GetVariable("GameState.lua", "spiceAmount", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) < ultimateSpiceCost) then
        DispatchGlobalEvent("Not_Enough_Spice", {"tonto"})
    end
end

function CastUltimate()
    if (ultimateTimer ~= nil) then
        Log("[FAIL] Ability Ultimate: Ability in cooldown!\n")
        do
            return
        end
    else
        if (target == nil) then
            target = GetGameObjectHovered()
        end
        if (target.tag ~= Tag.PLAYER) then
            Log("[FAIL] Ability Ultimate: You have to select an enemy first!\n")
            target = nil
            do
                return
            end
        end
        if (math.abs(Distance3D(target:GetTransform():GetPosition(), componentTransform:GetPosition())) <=
            ultimateCastRange) then
            if (componentAnimator ~= nil) then
                abilities.AbilityUltimate = AbilityStatus.Using
                DispatchGlobalEvent("Player_Ability", {characterID, Ability.Ultimate, abilities.AbilityUltimate})

                componentAnimator:SetSelectedClip("Point")

                StopMovement(false)

                LookAtTarget(target:GetTransform():GetPosition())

                trackList = {4, 5, 6}
                ChangeTrack(trackList)
            end
        else
            if (footstepsParticle ~= nil) then
                feetTimer = 0.5
                FootstepMovement()
            end
            destination = target:GetTransform():GetPosition()
            SetMovement(Movement.WALK)
            DispatchEvent("Pathfinder_UpdatePath", {{destination}, false, componentTransform:GetPosition()})
        end
    end
end

function DoUltimate()
    DispatchGlobalEvent("Used_Ultimate", {ultimateSpiceCost})

    if (GetVariable("GameState.lua", "GodMode", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == false) then
        -- Subtracts spice cost when using ultimate ability
        OGSpice = GetVariable("GameState.lua", "spiceAmount", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        NewSpice = OGSpice - ultimateSpiceCost
        SetVariable(NewSpice, "GameState.lua", "spiceAmount", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)

        str = "Spice Amount " .. NewSpice .. "\n"
        Log(str)
    end

    abilities.AbilityUltimate = AbilityStatus.Using
    DispatchGlobalEvent("Player_Ability", {characterID, Ability.Ultimate, abilities.AbilityUltimate})

    DispatchGlobalEvent("Sadiq_Update_Target", {target, 1}) -- fields[1] -> target; fields[2] -> targeted for (1 -> warning; 2 -> eat; 3 -> spit)
    StopMovement()
    componentAnimator:SetSelectedClip("PointToIdle")

    SetState(State.AIM_ULTIMATE_RECAST)
    abilities.AbilityUltimateRecast = AbilityStatus.Active -- Used this only for drawing
end

function RecastUltimate(isAlreadyCasted)

    local goHovered = GetGameObjectHovered()
    if (isAlreadyCasted == false) then
        -- GetGameObjectHovered() updates the last mouse click
        target = goHovered
        mouse = GetLastMouseClick()
        if (target.tag ~= Tag.FLOOR) then
            Log("[FAIL] Ability Ultimate: You have to select floor!\n")
            target = nil
            do
                return
            end
        end
        target = mouse
    end
    if (math.abs(Distance3D(target, componentTransform:GetPosition())) <= ultimateRecastRange) then
        if (componentAnimator ~= nil) then
            abilities.AbilityUltimateRecast = AbilityStatus.Casting -- Used this only for drawing

            componentAnimator:SetSelectedClip("Point")

            StopMovement(false)

            LookAtTarget(target)

            trackList = {4, 5, 6}
            ChangeTrack(trackList)

            ultimateTimer = 0.0
            abilities.AbilityUltimate = AbilityStatus.Cooldown
            DispatchGlobalEvent("Player_Ability",
                {characterID, Ability.Ultimate, abilities.AbilityUltimate, ultimateCooldown})
        end
    else
        if (footstepsParticle ~= nil) then
            feetTimer = 0.5
            FootstepMovement()
        end
        destination = target
        SetMovement(Movement.WALK)
        DispatchEvent("Pathfinder_UpdatePath", {{destination}, false, componentTransform:GetPosition()})
    end
end

function DoUltimateRecast()
    DispatchGlobalEvent("Sadiq_Update_Target", {target, 3}) -- fields[1] -> target; fields[2] -> targeted for (1 -> warning; 2 -> eat; 3 -> spit)

    abilities.AbilityUltimateRecast = AbilityStatus.Normal -- Used this only for drawing
    StopMovement(false)
    componentAnimator:SetSelectedClip("PointToIdle")
    SetState(State.IDLE)
end

function TakeDamage(damage)
    if (iFramesTimer ~= nil or currentHP == 0 or
        GetVariable("GameState.lua", "GodMode", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true) then
        do
            return
        end
    end

    iFramesTimer = 0
    bloodParticle:GetComponentParticle():ResumeParticleSpawn()

    if (damage == nil) then
        damage = 1
    end

    currentHP = currentHP - damage
    if (currentHP > 0) then
        Log("Omozra: Taking " .. damage .. " damage, current HP = " .. currentHP .. "\n")

        DispatchGlobalEvent("Player_Health", {characterID, currentHP, maxHP})

        trackList = {0, 1, 2}
        ChangeTrack(trackList)
    else
        currentHP = 0
        Log("Omozra: Dying\n")
        DispatchGlobalEvent("Player_Health", {characterID, currentHP, maxHP})
        Die()
    end
end

function Die()
    SetState(State.DEAD)
    currentHP = 0

    if (componentAnimator ~= nil) then
        componentAnimator:SetSelectedClip("Death")
    end
    if (currentTrackID ~= 3) then
        trackList = {3}
        ChangeTrack(trackList)
    end

    DispatchGlobalEvent("Player_Death", {characterID})
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
                TakeDamage(1)
            elseif (fields[2] == "Sardaukar") then
                TakeDamage(2)
            elseif (fields[2] == "Rabban") then
                TakeDamage(666)
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
            DispatchGlobalEvent("Player_Health", {characterID, currentHP, maxHP})
            -- If game changed to omozra, update HUD events depending on Abilities
            DispatchGlobalEvent("Player_Ability", {characterID, Ability.Primary, abilities.AbilityPrimary})
            -- Log("Omozra: Primary = " .. abilities.AbilityPrimary .. "\n")
            DispatchGlobalEvent("Player_Ability", {characterID, Ability.Secondary, abilities.AbilitySecondary})
            -- Log("Omozra: Secondary = " .. abilities.AbilitySecondary .. "\n")
            DispatchGlobalEvent("Player_Ability",
                {characterID, Ability.Ultimate, abilities.AbilityUltimate, ultimateTimer})
            -- Log("Omozra: Ultimate = " .. abilities.AbilityUltimate .. "\n")
            DispatchGlobalEvent("Omozra_Charges", {currentCharges, maxCharges})
        end
    elseif (key == "Dialogue_Opened") then
        isDialogueOpen = true
        StopMovement()
        SetState(State.IDLE)
        componentAnimator:SetSelectedClip("Idle")
    elseif (key == "Dialogue_Closed") then
        isDialogueOpen = false
    elseif (key == "Spice_Reward") then
        trackList = {7, 8}
        ChangeTrack(trackList)
    elseif (key == "Spit_Heal_Hit") then
        if (currentCharges >= primaryChargeCost) then
            abilities.AbilityPrimary = AbilityStatus.Normal
            DispatchGlobalEvent("Player_Ability", {characterID, Ability.Primary, abilities.AbilityPrimary})
        else
            abilities.AbilityPrimary = AbilityStatus.Disabled
            DispatchGlobalEvent("Player_Ability", {characterID, Ability.Primary, abilities.AbilityPrimary})
        end

        if currentCharges >= secondaryChargeCost then
            abilities.AbilitySecondary = AbilityStatus.Normal
            DispatchGlobalEvent("Player_Ability", {characterID, Ability.Secondary, abilities.AbilitySecondary})
        else
            abilities.AbilitySecondary = AbilityStatus.Disabled
            DispatchGlobalEvent("Player_Ability", {characterID, Ability.Secondary, abilities.AbilitySecondary})
        end

        if (fields[1] == gameObject) then
            if (currentHP < maxHP) then
                currentHP = currentHP + primaryHealAmount
                if currentHP > maxHP then
                    currentHP = maxHP
                end
                DispatchGlobalEvent("Player_Health", {characterID, currentHP, maxHP})
                Log("Sadiq has healed Omozra. Current HP = " .. currentHP .. "\n")

            else
                Log("Sadiq has healed Omozra, but it was already full HP\n")
            end
        end
    elseif key == "Sadiq_Update_Target" then
        if fields[2] == 2 or fields[2] == 4 then
            if (currentCharges >= primaryChargeCost) then
                abilities.AbilityPrimary = AbilityStatus.Normal
                DispatchGlobalEvent("Player_Ability", {characterID, Ability.Primary, abilities.AbilityPrimary})
            else
                abilities.AbilityPrimary = AbilityStatus.Disabled
                DispatchGlobalEvent("Player_Ability", {characterID, Ability.Primary, abilities.AbilityPrimary})
            end

            if currentCharges >= secondaryChargeCost then
                abilities.AbilitySecondary = AbilityStatus.Normal
                DispatchGlobalEvent("Player_Ability", {characterID, Ability.Secondary, abilities.AbilitySecondary})
            else
                abilities.AbilitySecondary = AbilityStatus.Disabled
                DispatchGlobalEvent("Player_Ability", {characterID, Ability.Secondary, abilities.AbilitySecondary})
            end
        end
    elseif (key == "Smokebomb_Start") then
        smokebombPosition = fields[1]
        smokebombRadius = fields[2]
    elseif (key == "Smokebomb_End") then
        smokebombPosition = nil
        smokebombRadius = nil
    elseif (key == "Update_Omozra_State") then -- fields 1 to 3: position
        -- fields 4 to 7: (primary, secondary, ultimate, passive)
        -- fields 8 and 9: health and charges

        componentRigidBody:SetRigidBodyPos(float3.new(fields[1], fields[2], fields[3]))

        if (fields[4] == 1) then
            primaryCastRange = 265
        elseif (fields[4] == 2) then
            primaryChargeCost = 5
        elseif (fields[4] == 3) then
            primaryHealAmount = 2
        end

        if (fields[5] == 1) then
            secondaryCastRange = 265
        elseif (fields[5] == 2) then

        elseif (fields[5] == 3) then

        end

        if (fields[6] == 1) then
            ultimateCastRange = 190
        elseif (fields[6] == 2) then
            ultimateCastRange = 225
        elseif (fields[6] == 3) then
            ultimateSpiceCost = 750
        end

        if (fields[7] == 1) then
            staminaSeconds = 7
            staminaTimer = staminaSeconds
        elseif (fields[7] == 2) then
            runMultiplierPercentage = 144
        elseif (fields[7] == 3) then
            maxHP = 4
        end

        currentHP = fields[8]
        DispatchGlobalEvent("Player_Health", {characterID, currentHP, maxHP})

        currentCharges = fields[9]
        DispatchGlobalEvent("Omozra_Charges", {currentCharges, maxCharges})

        Log("OMOZRA HEALTH POINTS: " .. currentHP .. "\n")

    elseif (key == "Omozra_Primary_Bugged") then
        currentCharges = currentCharges + primaryChargeCost
        abilities.AbilityPrimary = AbilityStatus.Normal
        DispatchGlobalEvent("Player_Ability", {characterID, Ability.Primary, abilities.AbilityPrimary})
        Log("Spit bugged, correction applied.\n")
    elseif (key == "Omozra_Secondary_Bugged") then
        currentCharges = currentCharges + secondaryChargeCost
        abilities.AbilitySecondary = AbilityStatus.Normal
        DispatchGlobalEvent("Player_Ability", {characterID, Ability.Secondary, abilities.AbilitySecondary})
        Log("Devour bugged, correction applied.\n")
    elseif (key == "Omozra_Ultimate_Bugged" or key == "Omozra_Ultimate_Recast_Bugged") then
        abilities.AbilityUltimate = AbilityStatus.Normal
        abilities.AbilityUltimateRecast = AbilityStatus.Normal
        DispatchGlobalEvent("Player_Ability", {characterID, Ability.Ultimate, abilities.AbilityUltimate})
        local OGSpice = GetVariable("GameState.lua", "spiceAmount", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        local NewSpice = OGSpice + ultimateSpiceCost
        SetVariable(NewSpice, "GameState.lua", "spiceAmount", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        Log("TP bugged, correction applied.\n")
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
        do
            return
        end
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

print("Omozra.lua compiled succesfully!\n")
Log("Omozra.lua compiled succesfully!\n")
