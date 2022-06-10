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
    ATTACK = 2,
    AIM_PRIMARY = 3,
    AIM_SECONDARY = 4,
    AIM_ULTIMATE = 5,
    MOSQUITO = 6,
    DEAD = 7,
    WORM = 8
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
    AbilityUltimate = AbilityStatus.Normal
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
characterID = 2
speed = 2140
crouchMultiplierPercentage = 63
runMultiplierPercentage = 138
staminaSeconds = 3
recoveryTime = 4
staminaTimer = staminaSeconds
standingStaminaMultiplier = 1.5
isTired = false

-- Basic Attack --
attackRange = 50.0
attackTime = 10.5
unawareChanceHarkAttack = 100
awareChanceHarkAttack = 75
aggroChanceHarkAttack = 50
unawareChanceSardAttack = 100
awareChanceSardAttack = 60
aggroChanceSardAttack = 40

-- Primary ability --
primaryCastRange = 165
primaryCooldown = 5
primarySpeed = 18000
primaryAuditoryDebuff = 30
primaryVisualDebuff = 100
unawareChanceHarkDart = 100
awareChanceHarkDart = 90
aggroChanceHarkDart = 0
unawareChanceSardDart = 100
awareChanceSardDart = 80
aggroChanceSardDart = 0

-- Secondary ability --
secondaryCastRange = 200
secondaryEffectRadius = 50
secondaryDuration = 5
maxSmokeBombCount = 3
smokeBombCount = maxSmokeBombCount
smokebombActive = false

-- Ultimate ability --
ultimateCastRange = 30
ultimateCooldown = 2
ultimateSpiceCost = 1000
ultimateKills = 1
ultimateLifeTime = 10
ultimateMaxDistance = 615 -- Careful changing variable name (HunterSeeker.lua)
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

--------------------Game Flow----------------------------
unlocked = false;

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

-- -- Primary ability --
-- primaryCastRangeIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
-- primaryCastRangeIV = InspectorVariable.new("primaryCastRange", primaryCastRangeIVT, primaryCastRange)
-- NewVariable(primaryCastRangeIV)

-- primaryCooldownIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
-- primaryCooldownIV = InspectorVariable.new("primaryCooldown", primaryCooldownIVT, primaryCooldown)
-- NewVariable(primaryCooldownIV)

-- dartSpeedIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
-- dartSpeedIV = InspectorVariable.new("dartSpeed", dartSpeedIVT, dartSpeed)
-- NewVariable(dartSpeedIV)

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
    bloodParticle = Find("Nerala Blood Particle")
    if (bloodParticle ~= nil) then
        bloodParticle:GetComponentParticle():StopParticleSpawn()
    end
    footstepsParticle = Find("Nerala Footstep Particle")
    deathParticle = Find("Nerala Mosquito Death Particle")
    if (deathParticle ~= nil) then
        deathParticle:GetComponentParticle():StopParticleSpawn()
    end
    -- Audio
    currentTrackID = -1

    -- HP
    currentHP = maxHP
    DispatchGlobalEvent("Player_Health", {characterID, currentHP, maxHP})

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

    if GetVariable("GameState.lua", "neralaAvailable", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true then
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
        if (currentState == State.ATTACK) then
            if (math.abs(Distance3D(componentTransform:GetPosition(), target:GetTransform():GetPosition())) <=
                attackRange) then
                Attack()
            else
                if (math.abs(Distance3D(destination, target:GetTransform():GetPosition())) >= 5) then
                    destination = target:GetTransform():GetPosition()
                    DispatchEvent("Pathfinder_UpdatePath", {{destination}, false, componentTransform:GetPosition()})
                end
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
            if (math.abs(Distance3D(componentTransform:GetPosition(), target)) <= secondaryCastRange) then
                if (componentAnimator ~= nil) then
                    CastSecondary(true)
                end
            else
                hasToMove = true
            end
        elseif (currentState == State.AIM_ULTIMATE) then
            if (math.abs(Distance3D(componentTransform:GetPosition(), target)) <= ultimateCastRange) then
                if (componentAnimator ~= nil) then
                    CastUltimate(true)
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

            -- Primary ability (Dart)
            if (currentState == State.AIM_PRIMARY) then
                CastPrimary()

                -- Secondary ability (Smokebomb)
            elseif (currentState == State.AIM_SECONDARY) then
                CastSecondary(false)

                -- Ultimate ability (mosquito)
            elseif (currentState == State.AIM_ULTIMATE) then
                CastUltimate(false)
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
                    if (goHit.tag == Tag.ENEMY and attackTimer == nil) then
                        SetState(State.ATTACK)
                        target = goHit
                        if (Distance3D(componentTransform:GetPosition(), goHit:GetTransform():GetPosition()) <=
                            attackRange) then
                            isMoving = false
                            Attack()
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
                    elseif (goHit.tag == Tag.PICKUP or goHit.tag == Tag.CORPSE) then
                        target = nil
                        currentState = State.IDLE
                        if (footstepsParticle ~= nil) then
                            feetTimer = 0.5
                            FootstepMovement()
                        end
                        destination = goHit:GetTransform():GetPosition()
                        DispatchEvent("Pathfinder_UpdatePath", {{destination}, false, componentTransform:GetPosition()})
                    elseif (goHit.tag == Tag.FLOOR) then
                        target = nil
                        currentState = State.IDLE
                        if (footstepsParticle ~= nil) then
                            feetTimer = 0.5
                            FootstepMovement()
                        end
                        destination = GetLastMouseClick()
                        DispatchEvent("Pathfinder_UpdatePath", {{destination}, false, componentTransform:GetPosition()})
                    else
                        if (isMoving == true) then
                            Log("No possible path\n")
                            do
                                return
                            end
                        else
                            Log("No possible path\n")
                            target = nil
                            if (footstepsParticle ~= nil) then
                                footstepsParticle:GetComponentParticle():StopParticleSpawn()
                            end
                            destination = nil
                            if (currentState ~= State.IDLE) then
                                SetState(State.IDLE)
                            end
                            isMoving = false
                            DispatchEvent("Pathfinder_UpdatePath",
                                {{destination}, false, componentTransform:GetPosition()})
                        end

                    end
                    if (currentMovement == Movement.WALK and isDoubleClicking == true and isMoving == true and
                        not isTired) then
                        SetMovement(Movement.RUN)
                    else
                        if (currentMovement == Movement.IDLE and isMoving == true) then
                            SetMovement(Movement.WALK)
                        end
                        isDoubleClicking = true
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
        if (GetInput(21) == KEY_STATE.KEY_DOWN) then
            ActivePrimary()
        end

        -- 2
        if (GetInput(22) == KEY_STATE.KEY_DOWN) then
            ActiveSecondary()
        end

        -- 3
        if (GetInput(23) == KEY_STATE.KEY_DOWN) then
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
end
--------------------------------------------------

------------------- Functions --------------------
function SetState(newState)
    if (newState == State.IDLE) then
        currentState = State.IDLE
    elseif (newState == State.ATTACK) then
        currentState = State.ATTACK
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
    elseif (newState == State.DEAD) then
        currentState = State.DEAD
        StopMovement()
    elseif (newState == State.WORM) then
        currentState = State.WORM
        StopMovement()
    elseif (newState == State.MOSQUITO) then
        currentState = State.MOSQUITO
        StopMovement(false)
    end
end

function SetMovement(newMovement)
    if (newMovement == Movement.IDLE) then
        currentMovement = Movement.IDLE
        if (componentSwitch ~= nil) then
            if (currentTrackID ~= -1) then
                componentSwitch:StopTrack(currentTrackID)
                currentTrackID = -1
            end
        end
    elseif (newMovement == Movement.WALK) then
        currentMovement = Movement.WALK
        if (componentAnimator ~= nil) then
            componentAnimator:SetSelectedClip("Walk")
        end
        trackList = {12}
        ChangeTrack(trackList)
    elseif (newMovement == Movement.RUN) then
        currentMovement = Movement.RUN
        if (componentAnimator ~= nil) then
            componentAnimator:SetSelectedClip("Run")
        end
        trackList = {13}
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
        if (currentMovement ~= Movement.IDLE) then
            trackList = {12}
            ChangeTrack(trackList)
        end
        if (componentAnimator ~= nil) then
            componentAnimator:SetSelectedClip("Crouch")
        end
    end
end

function CancelAbilities(onlyAbilities)
    if (currentState == State.AIM_PRIMARY and abilities.AbilityPrimary == AbilityStatus.Active) then
        abilities.AbilityPrimary = AbilityStatus.Normal
        DispatchGlobalEvent("Player_Ability", {characterID, Ability.Primary, abilities.AbilityPrimary})
        DispatchGlobalEvent("Hover_End", {})
    elseif (currentState == State.AIM_SECONDARY and abilities.AbilitySecondary == AbilityStatus.Active) then
        abilities.AbilitySecondary = AbilityStatus.Normal
        DispatchGlobalEvent("Player_Ability", {characterID, Ability.Secondary, abilities.AbilitySecondary})
    elseif (currentState == State.AIM_ULTIMATE and abilities.AbilityUltimate == AbilityStatus.Active) then
        abilities.AbilityUltimate = AbilityStatus.Normal
        DispatchGlobalEvent("Player_Ability", {characterID, Ability.Ultimate, abilities.AbilityUltimate})
    end

    if (onlyAbilities == nil) then
        if componentAnimator ~= nil then
            if (currentMovement == Movement.IDLE_CROUCH) then
                componentAnimator:SetSelectedClip("IdleCrouch")
            else
                componentAnimator:SetSelectedClip("Idle")
            end
        end

        if (currentState ~= State.WORM) then
            SetState(State.IDLE)
        end
        StopMovement()
    end
end

isHoveringEnemy = nil
lastEnemyTarget = nil
function DrawHoverParticle()
    if (choosingTargetParticle == nil) then
        do
            return
        end
    end

    if (isSelected == true) then
        local drawingTarget = GetGameObjectHovered()
        local finalPosition
        if ((currentState == Sta or currentState == State.AIM_PRIMARY or currentState == State.AIM_SECONDARY or
            currentState == State.AIM_ULTIMATE) and target ~= nil) then
            if (target.x == nil) then
                t = target:GetTransform():GetPosition()
            else
                t = target
            end
            choosingTargetParticle:GetComponentParticle():SetColor(255, 0, 255, 255)
            finalPosition = float3.new(t.x, t.y + 1, t.z)
            lastEnemyTarget = drawingTarget
            if isHoveringEnemy ~= nil then
                DispatchGlobalEvent("Not_Hovering_Enemy", {lastEnemyTarget})
                isHoveringEnemy = nil
            end
        elseif (currentState == State.AIM_PRIMARY and drawingTarget.tag == Tag.ENEMY) then
            lastEnemyTarget = drawingTarget
            local dist = Distance3D(drawingTarget:GetTransform():GetPosition(), componentTransform:GetPosition())
            if (currentState == State.AIM_PRIMARY and dist <= primaryCastRange) then
                choosingTargetParticle:GetComponentParticle():SetColor(0, 255, 0, 255)
            else
                choosingTargetParticle:GetComponentParticle():SetColor(255, 0, 0, 255)
            end
            finalPosition = drawingTarget:GetTransform():GetPosition()
            finalPosition.y = finalPosition.y + 1

            if isHoveringEnemy == nil then
                if currentState == State.AIM_PRIMARY then
                    DispatchGlobalEvent("Hovering_Enemy", {drawingTarget, "Knife"})
                end
                isHoveringEnemy = true
            end
        elseif ((currentState == State.AIM_SECONDARY or currentState == State.AIM_ULTIMATE) and drawingTarget.tag ==
            Tag.FLOOR) then
            local mouseClick = GetLastMouseClick()
            if ((currentState == State.AIM_SECONDARY and Distance3D(mouseClick, componentTransform:GetPosition()) <=
                secondaryCastRange) or
                (currentState == State.AIM_ULTIMATE and Distance3D(mouseClick, componentTransform:GetPosition()) <=
                    ultimateCastRange)) then
                choosingTargetParticle:GetComponentParticle():SetColor(0, 255, 0, 255)
            else
                choosingTargetParticle:GetComponentParticle():SetColor(255, 0, 0, 255)
            end
            -- This is only 1 instead of mouseClick.y + 1 because if hovering game objects with height like characters, the hovering particle will go up
            finalPosition = float3.new(mouseClick.x, 1, mouseClick.z)
        else
            if isHoveringEnemy ~= nil then
                Log("Sending is not hovering event\n")
                DispatchGlobalEvent("Not_Hovering_Enemy", {lastEnemyTarget})
                isHoveringEnemy = nil
            end

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
                componentLight:SetDiffuse(0.3)
            elseif (abilities.AbilitySecondary == AbilityStatus.Active) then
                componentLight:SetRange(secondaryCastRange)
                componentLight:SetAngle(360 / 2)
                componentLight:SetDiffuse(0.3)
            elseif (abilities.AbilityUltimate == AbilityStatus.Active) then
                componentLight:SetRange(ultimateCastRange)
                componentLight:SetAngle(360 / 2)
                componentLight:SetDiffuse(0.3)
            elseif (abilities.AbilityUltimate == AbilityStatus.Using) then
                componentLight:SetRange(ultimateMaxDistance)
                componentLight:SetAngle(360 / 2)
                componentLight:SetDiffuse(0.2)
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

    -- Basic attack cooldown
    if (attackTimer ~= nil) then
        attackTimer = attackTimer + dt
        if (attackTimer >= attackTime) then
            attackTimer = nil
        end
    end

    -- Primary ability cooldown
    if (primaryTimer ~= nil) then

        primaryTimer = primaryTimer + dt
        if (primaryTimer >= primaryCooldown) then
            primaryTimer = nil
            abilities.AbilityPrimary = AbilityStatus.Normal
            DispatchGlobalEvent("Player_Ability", {characterID, Ability.Primary, abilities.AbilityPrimary})
        end

    end

    -- Secondary ability cooldown
    -- if (secondaryTimer ~= nil) then
    --     secondaryTimer = secondaryTimer + dt
    --     if (secondaryTimer >= secondaryCooldown) then
    --         secondaryTimer = nil
    --         abilities.AbilitySecondary = AbilityStatus.Normal
    --         DispatchGlobalEvent("Player_Ability", {characterID, Ability.Secondary, abilities.AbilitySecondary})
    --     end
    -- end

    -- Ultimate ability cooldown
    if (ultimateTimer ~= nil) then
        ultimateTimer = ultimateTimer + dt
        if (ultimateTimer >= ultimateCooldown) then
            ultimateTimer = nil
            abilities.AbilityUltimate = AbilityStatus.Normal
            DispatchGlobalEvent("Player_Ability", {characterID, Ability.Ultimate, abilities.AbilityUltimate})
        end
    end

    -- Animation timer
    if (componentAnimator ~= nil) then
        if (componentAnimator:IsCurrentClipLooping() == false) then
            if (componentAnimator:IsCurrentClipPlaying() == true) then
                ret = false
            else
                if (currentState == State.ATTACK) then
                    DoAttack()
                elseif (currentState == State.AIM_PRIMARY) then
                    FireDart()
                elseif (currentState == State.AIM_SECONDARY) then
                    PlaceSmokebomb()
                elseif (currentState == State.AIM_ULTIMATE) then
                    DoUltimate()
                elseif (currentState == State.MOSQUITO) then
                    ret = false
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

    if (mosquitoDeathParticleTimer ~= nil) then
        mosquitoDeathParticleTimer = mosquitoDeathParticleTimer + dt
        if (mosquitoDeathParticleTimer > 0.5) then
            if (deathParticle ~= nil) then
                deathParticle:GetComponentParticle():StopParticleSpawn()
            end
            mosquitoDeathParticleTimer = nil
        end
    end

    -- If she's dead she can't do anything
    if (currentState == State.DEAD or currentState == State.WORM) then
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

-- Basic Attack
function Attack()
    SetState(State.ATTACK)
    componentAnimator:SetSelectedClip("Attack")
    -- impactParticle:GetComponentParticle():SetLoop(true)
    -- impactParticle:GetTransform():SetPosition(float3.new(target:GetTransform():GetPosition().x,
    --     target:GetTransform():GetPosition().y + 17, target:GetTransform():GetPosition().z + 5))

    LookAtTarget(target:GetTransform():GetPosition())
end

function DoAttack()

    componentAnimator:SetSelectedClip("AttackToIdle")

    DispatchGlobalEvent("Player_Attack", {target, characterID})
    DispatchGlobalEvent("Auditory_Trigger", {componentTransform:GetPosition(), 100, "single", gameObject})

    LookAtTarget(target:GetTransform():GetPosition())

    trackList = {0, 1}
    ChangeTrack(trackList)

    attackTimer = 0.0

    target = nil
    -- impactParticle:GetComponentParticle():SetLoop(false)

    SetState(State.IDLE)
end

-- Primary ability
function ActivePrimary()
    if (primaryTimer == nil) then
        if (currentState == State.AIM_PRIMARY) then
            CancelAbilities()
        else
            CancelAbilities()
            SetState(State.AIM_PRIMARY)
            abilities.AbilityPrimary = AbilityStatus.Active
            DispatchGlobalEvent("Player_Ability", {characterID, Ability.Primary, abilities.AbilityPrimary})
            DispatchGlobalEvent("Hover_Start", {"Dart"})
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
        if (target.tag ~= Tag.ENEMY) then
            Log("[FAIL] Ability Primary: You have to select an enemy first!\n")
            target = nil
            do
                return
            end
        else
            DispatchGlobalEvent("Hover_End", {})
            if (math.abs(Distance3D(target:GetTransform():GetPosition(), componentTransform:GetPosition())) <=
                primaryCastRange) then
                if (componentAnimator ~= nil) then
                    abilities.AbilityPrimary = AbilityStatus.Using
                    DispatchGlobalEvent("Player_Ability", {characterID, Ability.Primary, abilities.AbilityPrimary})

                    componentAnimator:SetSelectedClip("Dart")
                    StopMovement(false)

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
end

function FireDart()

    InstantiatePrefab("Dart2")

    primaryTimer = 0.0
    abilities.AbilityPrimary = AbilityStatus.Cooldown
    DispatchGlobalEvent("Player_Ability", {characterID, Ability.Primary, abilities.AbilityPrimary, primaryCooldown})

    trackList = {6, 7}
    ChangeTrack(trackList)

    componentAnimator:SetSelectedClip("DartToIdle")
    SetState(State.IDLE)
end

-- Secondary ability
function ActiveSecondary()
    if (smokeBombCount > 0 and smokebombActive == false) then
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

function CastSecondary(isAlreadyCasted)
    if (smokeBombCount <= 0) then
        Log("[FAIL] Ability Secondary: You don't have enough smokebombs!\n")
        do
            return
        end
    else
        local goHovered = GetGameObjectHovered()
        if (isAlreadyCasted == false) then
            -- GetGameObjectHovered() updates the last mouse click
            target = goHovered
            mouse = GetLastMouseClick()
            if (target.tag ~= Tag.FLOOR) then
                Log("[FAIL] Ability Secondary: You have to select floor!\n")
                target = nil
                do
                    return
                end
            end
            target = mouse
        end
        if (math.abs(Distance3D(target, componentTransform:GetPosition())) <= secondaryCastRange) then
            if (componentAnimator ~= nil) then
                abilities.AbilitySecondary = AbilityStatus.Using
                DispatchGlobalEvent("Player_Ability", {characterID, Ability.Secondary, abilities.AbilitySecondary})

                componentAnimator:SetSelectedClip("Smokebomb")
                StopMovement(false)

                LookAtTarget(target)
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
end

function PlaceSmokebomb()

    InstantiatePrefab("Smokebomb")
    smokeBombCount = smokeBombCount - 1

    -- secondaryTimer = 0.0

    trackList = {8}
    ChangeTrack(trackList)

    componentAnimator:SetSelectedClip("SmokebombToIdle")
    SetState(State.IDLE)
end

-- Ultimate ability
function ActiveUltimate()
    if (ultimateTimer == nil and currentState ~= State.MOSQUITO and
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

function CastUltimate(isAlreadyCasted)
    if (ultimateTimer ~= nil) then
        Log("[FAIL] Ability Ultimate: Ability in cooldown!\n")
        do
            return
        end
    else
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
        if (math.abs(Distance3D(target, componentTransform:GetPosition())) <= ultimateCastRange) then
            if (componentAnimator ~= nil) then
                abilities.AbilityUltimate = AbilityStatus.Using
                DispatchGlobalEvent("Player_Ability", {characterID, Ability.Ultimate, abilities.AbilityUltimate})

                componentAnimator:SetSelectedClip("Mosquito")

                StopMovement(false)

                LookAtTarget(target)

                trackList = {9}
                ChangeTrack(trackList)
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
end

function DoUltimate()

    DispatchGlobalEvent("Used_Ultimate", {ultimateSpiceCost})

    if (GetVariable("GameState.lua", "GodMode", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == false) then
        -- Subtracts spice cost when using ultimate ability
        local OGSpice = GetVariable("GameState.lua", "spiceAmount", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        local NewSpice = OGSpice - ultimateSpiceCost
        SetVariable(NewSpice, "GameState.lua", "spiceAmount", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)

        local str = "Spice Amount " .. NewSpice .. "\n"
        Log(str)
    end

    InstantiatePrefab("Mosquito")

    -- No new clip, the last clip has to last until the mosquito dies
    SetState(State.MOSQUITO)
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
        Log("Nerala: Taking " .. damage .. " damage, current HP = " .. currentHP .. "\n")

        DispatchGlobalEvent("Player_Health", {characterID, currentHP, maxHP})

        trackList = {2, 3, 4}
        ChangeTrack(trackList)
    else
        currentHP = 0
        Log("Nerala: Dying\n")
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
        trackList = {5}
        ChangeTrack(trackList)
    end

    SetVariable(0, "GameState.lua", "gameOverTimer", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
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
            -- If nerala is being changed
            CancelAbilities(true)
        end
        if (fields[2] == characterID) then
            DispatchGlobalEvent("Player_Health", {characterID, currentHP, maxHP})
            -- If game changed to nerala, update HUD events depending on Abilities
            DispatchGlobalEvent("Player_Ability", {characterID, Ability.Primary, abilities.AbilityPrimary, primaryTimer})
            -- Log("Nerala: Primary = " .. abilities.AbilityPrimary .. "\n")
            DispatchGlobalEvent("Player_Ability", {characterID, Ability.Secondary, abilities.AbilitySecondary})
            -- Log("Nerala: Secondary = " .. abilities.AbilitySecondary .. "\n")
            DispatchGlobalEvent("Player_Ability",
                {characterID, Ability.Ultimate, abilities.AbilityUltimate, ultimateTimer})
            -- Log("Nerala: Ultimate = " .. abilities.AbilityUltimate .. "\n")
        end
    elseif (key == "Mosquito_Death") then
        trackList = {14}
        ChangeTrack(trackList)
        ultimateTimer = 0.0
        mosquitoDeathParticleTimer = 0.0
        abilities.AbilityUltimate = AbilityStatus.Cooldown
        DispatchGlobalEvent("Player_Ability",
            {characterID, Ability.Ultimate, abilities.AbilityUltimate, ultimateCooldown})
        SetState(State.IDLE)
    elseif (key == "Sadiq_Update_Target") then -- fields[1] -> target; targeted for (1 -> warning; 2 -> eat; 3 -> spit)
        if (fields[1] == gameObject) then
            if (fields[2] == 1) then
                SetState(State.WORM)
                if (componentAnimator ~= nil) then
                    componentAnimator:SetSelectedClip("Idle")
                end
            elseif (fields[2] == 2) then
                if (componentRigidBody ~= nil) then
                    componentRigidBody:SetRigidBodyPos(float3.new(componentTransform:GetPosition().x, -50,
                        componentTransform:GetPosition().z))
                end
                gameObject.active = false
                DispatchGlobalEvent("Disable_Character", {characterID})
            end
        elseif (currentState == State.WORM and fields[2] == nil) then

            if (componentRigidBody ~= nil) then
                componentRigidBody:SetRigidBodyPos(fields[1])
            end
            gameObject.active = true
            SetState(State.IDLE)
            DispatchGlobalEvent("Enable_Character", {characterID})
        end
    elseif (key == "Dialogue_Opened") then
        isDialogueOpen = true
        StopMovement()
        SetState(State.IDLE)
        componentAnimator:SetSelectedClip("Idle")
    elseif (key == "Dialogue_Closed") then
        isDialogueOpen = false
    elseif (key == "Spice_Reward") then
        trackList = {10, 11}
        ChangeTrack(trackList)
    elseif (key == "Spit_Heal_Hit") then
        if (fields[1] == gameObject) then
            if (currentHP < maxHP) then
                currentHP = currentHP +
                                GetVariable("Omozra.lua", "primaryHealAmount", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
                if currentHP > maxHP then
                    currentHP = maxHP
                end
                DispatchGlobalEvent("Player_Health", {characterID, currentHP, maxHP})
                Log("Sadiq has healed Nerala. Current HP = " .. currentHP .. "\n")
            else
                Log("Sadiq has healed Nerala, but it was already full HP\n")
            end
        end
    elseif (key == "Smokebomb_Start") then
        smokebombPosition = fields[1]
        smokebombRadius = fields[2]
        smokebombActive = true
    elseif (key == "Smokebomb_End") then
        smokebombPosition = nil
        smokebombRadius = nil
        smokebombActive = false
        if (smokeBombCount > 0) then
            abilities.AbilitySecondary = AbilityStatus.Normal
            DispatchGlobalEvent("Player_Ability", {characterID, Ability.Secondary, abilities.AbilitySecondary})
        else
            abilities.AbilitySecondary = AbilityStatus.Disabled -- Should be state disabled 
            DispatchGlobalEvent("Player_Ability", {characterID, Ability.Secondary, abilities.AbilitySecondary})
        end
    elseif (key == "Update_Nerala_State") then -- fields 1 to 3: position
        -- fields 4 to 7: (primary, secondary, ultimate, passive)
        -- field 8: health

        componentRigidBody:SetRigidBodyPos(float3.new(fields[1], fields[2], fields[3]))

        if (fields[4] == 1) then
            primaryCastRange = 195
        elseif (fields[4] == 2) then
            unawareChanceHarkDart = 100
            awareChanceHarkDart = 100
            aggroChanceHarkDart = 0
            unawareChanceSardDart = 100
            awareChanceSardDart = 90
            aggroChanceSardDart = 0
        elseif (fields[4] == 3) then
            primaryVisualDebuff = 75
        end

        if (fields[5] == 1) then
            secondaryCastRange = 265
            secondaryEffectRadius = 60
        elseif (fields[5] == 2) then
            secondaryDuration = 7
        elseif (fields[5] == 3) then
            maxSmokeBombCount = 5
            smokeBombCount = maxSmokeBombCount
        end

        if (fields[6] == 1) then
            ultimateMaxDistance = 735
        elseif (fields[6] == 2) then
            ultimateLifeTime = 15
        elseif (fields[6] == 3) then
            ultimateKills = 2
            ultimateSpiceCost = 750
        end

        if (fields[7] == 1) then
            staminaSeconds = 5
            staminaTimer = staminaSeconds
        elseif (fields[7] == 2) then
            attackTime = 8
        elseif (fields[7] == 3) then
            crouchMultiplierPercentage = 75
        end

        currentHP = fields[8]
        DispatchGlobalEvent("Player_Health", {characterID, currentHP, maxHP})

        Log("NERALA HEALTH POINTS: " .. currentHP .. "\n")

    elseif (key == "Nerala_Primary_Bugged") then
        primaryTimer = nil
        abilities.AbilityPrimary = AbilityStatus.Normal
        DispatchGlobalEvent("Player_Ability", {characterID, Ability.Primary, abilities.AbilityPrimary})
        Log("Dart bugged, correction applied.\n")
    elseif (key == "Nerala_Secondary_Bugged") then
        secondaryTimer = nil
        abilities.AbilitySecondary = AbilityStatus.Normal
        DispatchGlobalEvent("Player_Ability", {characterID, Ability.Secondary, abilities.AbilitySecondary})
        smokeBombCount = smokeBombCount + 1
        Log("Smokebomb bugged, correction applied.\n")
    elseif (key == "Nerala_Ultimate_Bugged") then
        ultimateTimer = nil
        abilities.AbilityUltimate = AbilityStatus.Normal
        DispatchGlobalEvent("Player_Ability", {characterID, Ability.Ultimate, abilities.AbilityUltimate})
        local OGSpice = GetVariable("GameState.lua", "spiceAmount", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        local NewSpice = OGSpice + ultimateSpiceCost
        SetVariable(NewSpice, "GameState.lua", "spiceAmount", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        Log("Mosquito bugged, correction applied.\n")
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

print("Nerala.lua compiled succesfully!\n")
Log("Nerala.lua compiled succesfully!\n")

