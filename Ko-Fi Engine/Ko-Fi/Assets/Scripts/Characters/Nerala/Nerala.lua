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
speed = 2500
crouchMultiplierPercentage = 66
runMultiplierPercentage = 150
staminaSeconds = 5
recoveryTime = 5
staminaTimer = staminaSeconds
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
primaryCastRange = 100
primaryCooldown = 5
dartSpeed = 10000
unawareChanceHarkDart = 100
awareChanceHarkDart = 90
aggroChanceHarkDart = 0
unawareChanceSardDart = 100
awareChanceSardDart = 80
aggroChanceSardDart = 0

-- Secondary ability --
secondaryCastRange = 75
secondaryCooldown = 10.0
maxSmokeBombCount = 3
smokeBombCount = maxSmokeBombCount
smokebombActive = false

-- Ultimate ability --
ultimateCastRange = 50
ultimateCooldown = 30.0
ultimateSpiceCost = 1500
ultimateMaxDistance = 300 -- Careful changing variable name (HunterSeeker.lua)
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
    mouseParticles = Find("Mouse Particle")
    if (mouseParticles ~= nil) then
        mouseParticles:GetComponentParticle():StopParticleSpawn()
    end
    choosingTargetParticle = Find("Target Particle")
    bloodParticle = Find("Nerala Blood Particle")
    if (bloodParticle ~= nil) then
        bloodParticle:GetComponentParticle():StopParticleSpawn()
    end
    impactParticle = Find("Nerala Impact Particle") -- not used currently
    swooshParticle = Find("Nerala Swoosh Particle") -- not used currently
    footstepsParticle = Find("Nerala Footstep Particle")

    -- Audio
    currentTrackID = -1

    -- HP
    currentHP = maxHP
    DispatchGlobalEvent("Player_Health", {characterID, currentHP, maxHP})
end

-- Called each loop iteration
function Update(dt)

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
    if (currentState == State.ATTACK) then
        if (Distance3D(componentTransform:GetPosition(), target:GetTransform():GetPosition()) <= attackRange) then
            Attack()
        else
            destination = target:GetTransform():GetPosition()
            hasToMove = true
        end
    elseif (currentState == State.AIM_PRIMARY or currentState == State.AIM_SECONDARY or currentState ==
        State.AIM_ULTIMATE) then
        StopMovement()
        componentAnimator:SetSelectedClip("Idle")
    elseif (destination ~= nil) then
        hasToMove = true
    end

    -- Gather Inputs
    if (IsSelected() == true) then

        if staminaBarBlue == nil then
            ConfigStaminaBars()
        else
            UpdateStaminaBar()
        end

        -- Left Click
        if (GetInput(1) == KEY_STATE.KEY_DOWN) then

            -- Primary ability (Dart)
            if (currentState == State.AIM_PRIMARY) then
                if (primaryTimer ~= nil) then
                    Log("[FAIL] Ability Primary: Ability in cooldown!\n")
                else
                    target = GetGameObjectHovered()
                    if (target.tag ~= Tag.ENEMY) then
                        Log("[FAIL] Ability Primary: You have to select an enemy first!\n")
                        target = nil
                    else
                        if (Distance3D(target:GetTransform():GetPosition(), componentTransform:GetPosition()) >
                            primaryCastRange) then
                            Log("[FAIL] Ability Primary: Ability out of range!\n")
                            target = nil
                        else
                            if (componentAnimator ~= nil) then
                                CastPrimary(target:GetTransform():GetPosition())
                            end
                        end
                    end
                end

                -- Secondary ability (Smokebomb)
            elseif (currentState == State.AIM_SECONDARY) then
                if (smokeBombCount <= 0) then -- secondaryTimer ~= nil
                    Log("[FAIL] Ability Secondary: You don't have enough smoke bombs!\n")
                else
                    target = GetGameObjectHovered() -- GetGameObjectHovered updates the last mouse click
                    local mouse = GetLastMouseClick()
                    if (Distance3D(mouse, componentTransform:GetPosition()) > secondaryCastRange) then
                        Log("[FAIL] Ability Secondary: Ability out of range!\n")
                        target = nil
                    else
                        if (target.tag ~= Tag.FLOOR) then
                            Log("[FAIL] Ability Secondary: You have to select floor!\n")
                            target = nil
                        else
                            target = mouse
                            if (componentAnimator ~= nil) then
                                CastSecondary(mouse)
                            end
                        end
                    end
                end

                -- Ultimate ability (mosquito)
            elseif (currentState == State.AIM_ULTIMATE) then
                if (ultimateTimer ~= nil) then
                    Log("[FAIL] Ability Ultimate: Ability in cooldown!\n")
                else
                    target = GetGameObjectHovered() -- GetGameObjectHovered updates the last mouse click
                    local mouse = GetLastMouseClick()
                    if (Distance3D(mouse, componentTransform:GetPosition()) > ultimateCastRange) then
                        Log("[FAIL] Ability Ultimate: Ability out of range!\n")
                        target = nil
                    else
                        if (target.tag ~= Tag.FLOOR) then
                            Log("[FAIL] Ability Ultimate: You have to select floor!\n")
                            target = nil
                        else
                            target = mouse
                            if (componentAnimator ~= nil) then
                                CastUltimate(mouse)
                            end
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
                    if (goHit.tag == Tag.ENEMY and attackTimer == nil) then
                        SetState(State.ATTACK)
                        target = goHit
                        if (Distance3D(componentTransform:GetPosition(), goHit:GetTransform():GetPosition()) <=
                            attackRange) then
                            isMoving = false
                            Attack()
                        else
                            if (footstepsParticle ~= nil) then
                                footstepsParticle:GetComponentParticle():ResumeParticleSpawn()
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
                            footstepsParticle:GetComponentParticle():ResumeParticleSpawn()
                        end
                        destination = goHit:GetTransform():GetPosition()
                        DispatchEvent("Pathfinder_UpdatePath", {{destination}, false, componentTransform:GetPosition()})
                    elseif (goHit.tag == Tag.FLOOR) then
                        target = nil
                        currentState = State.IDLE
                        if (footstepsParticle ~= nil) then
                            footstepsParticle:GetComponentParticle():ResumeParticleSpawn()
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

        -- T
        if (GetInput(13) == KEY_STATE.KEY_DOWN) then
            currentHP = 0
            DispatchGlobalEvent("Player_Health", {characterID, currentHP, maxHP})
            Die()
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
    elseif abilities.AbilitySecondary == AbilityStatus.Using then
        isUsingW = true
    elseif abilities.AbilityUltimate == AbilityStatus.Using then
        isUsingE = true
    else
        isUsingQ = false
        isUsingW = false
        isUsingE = false
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
        trackList = {0}
        ChangeTrack(trackList)
    elseif (newMovement == Movement.RUN) then
        currentMovement = Movement.RUN
        if (componentAnimator ~= nil) then
            componentAnimator:SetSelectedClip("Run")
        end
        trackList = {1}
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
            trackList = {0}
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
    elseif (currentState == State.AIM_SECONDARY and abilities.AbilitySecondary == AbilityStatus.Active) then
        abilities.AbilitySecondary = AbilityStatus.Normal
        DispatchGlobalEvent("Player_Ability", {characterID, Ability.Secondary, abilities.AbilitySecondary})
    elseif (currentState == State.AIM_ULTIMATE and abilities.AbilityUltimate == AbilityStatus.Active) then
        abilities.AbilityUltimate = AbilityStatus.Normal
        DispatchGlobalEvent("Player_Ability", {characterID, Ability.Ultimate, abilities.AbilityUltimate})
    end

    if (onlyAbilities == nil) then
        if (currentState ~= State.WORM) then
            SetState(State.IDLE)
        end
    end
end

function DrawHoverParticle()
    if (choosingTargetParticle == nil) then
        do
            return
        end
    end

    if (IsSelected() == true) then
        local drawingTarget = GetGameObjectHovered()
        local finalPosition
        if ((currentState == State.ATTACK or currentState == State.AIM_PRIMARY or currentState == State.AIM_SECONDARY or
            currentState == State.AIM_ULTIMATE) and target ~= nil) then
            if (target.x == nil) then
                t = target:GetTransform():GetPosition()
            else
                t = target
            end
            choosingTargetParticle:GetComponentParticle():SetColor(255, 0, 255, 255)
            finalPosition = float3.new(t.x, t.y + 1, t.z)
        elseif (currentState == State.AIM_PRIMARY and drawingTarget.tag == Tag.ENEMY) then
            local dist = Distance3D(drawingTarget:GetTransform():GetPosition(), componentTransform:GetPosition())
            if (currentState == State.AIM_PRIMARY and dist <= primaryCastRange) then
                choosingTargetParticle:GetComponentParticle():SetColor(0, 255, 0, 255)
            else
                choosingTargetParticle:GetComponentParticle():SetColor(255, 0, 0, 255)
            end
            finalPosition = drawingTarget:GetTransform():GetPosition()
            finalPosition.y = finalPosition.y + 1
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
        if (IsSelected() == true) then
            if (abilities.AbilityPrimary == AbilityStatus.Active) then
                componentLight:SetRange(primaryCastRange)
                componentLight:SetAngle(360 / 2)
                componentLight:SetDiffuse(0.2)
            elseif (abilities.AbilitySecondary == AbilityStatus.Active) then
                componentLight:SetRange(secondaryCastRange)
                componentLight:SetAngle(360 / 2)
                componentLight:SetDiffuse(0.2)
            elseif (abilities.AbilityUltimate == AbilityStatus.Active) then
                componentLight:SetRange(ultimateCastRange)
                componentLight:SetAngle(360 / 2)
                componentLight:SetDiffuse(0.2)
            elseif (abilities.AbilityUltimate == AbilityStatus.Using) then
                componentLight:SetRange(ultimateMaxDistance)
                componentLight:SetAngle(360 / 2)
                componentLight:SetDiffuse(0.1)
            else
                componentLight:SetAngle(0)
            end
        else
            componentLight:SetAngle(0)
        end
    end
end

function UpdateStaminaBar()
    local proportion = staminaTimer / staminaSeconds
    local recoveryProportion = staminaTimer / recoveryTime

    local pos = componentTransform:GetPosition()

    staminaBarGreen:GetTransform():SetPosition(float3.new(pos.x, pos.y + 30, pos.z))
    staminaBarYellow:GetTransform():SetPosition(float3.new(pos.x, pos.y + 30, pos.z))
    staminaBarRed:GetTransform():SetPosition(float3.new(pos.x, pos.y + 30, pos.z))
    staminaBarBlue:GetTransform():SetPosition(float3.new(pos.x, pos.y + 30, pos.z))

    -- NEW
    if isTired == false then
        if proportion >= 0.66 then
            staminaBarGreen:GetTransform():SetScale(float3.new(staminaBarSizeX, staminaBarSizeY * (proportion),
                staminaBarSizeZ))
            staminaBarYellow:GetTransform():SetScale(float3.new(0, 0, 0))
            staminaBarRed:GetTransform():SetScale(float3.new(0, 0, 0))
            staminaBarBlue:GetTransform():SetScale(float3.new(0, 0, 0))
        elseif proportion >= 0.33 and proportion < 0.66 then
            staminaBarGreen:GetTransform():SetScale(float3.new(0, 0, 0))
            staminaBarYellow:GetTransform():SetScale(float3.new(staminaBarSizeX, staminaBarSizeY * (proportion),
                staminaBarSizeZ))
            staminaBarRed:GetTransform():SetScale(float3.new(0, 0, 0))
            staminaBarBlue:GetTransform():SetScale(float3.new(0, 0, 0))
        else
            staminaBarGreen:GetTransform():SetScale(float3.new(0, 0, 0))
            staminaBarYellow:GetTransform():SetScale(float3.new(0, 0, 0))
            staminaBarRed:GetTransform():SetScale(float3.new(staminaBarSizeX, staminaBarSizeY * (proportion),
                staminaBarSizeZ))
            staminaBarBlue:GetTransform():SetScale(float3.new(0, 0, 0))
        end
    else
        staminaBarGreen:GetTransform():SetScale(float3.new(0, 0, 0))
        staminaBarYellow:GetTransform():SetScale(float3.new(0, 0, 0))
        staminaBarRed:GetTransform():SetScale(float3.new(0, 0, 0))
        staminaBarBlue:GetTransform():SetScale(float3.new(staminaBarSizeX, staminaBarSizeY * (recoveryProportion),
            staminaBarSizeZ))
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
                    componentAnimator:SetSelectedClip("Idle") -- Comment this line to test animations in-game
                end
            end
        end
    end

    -- If she's dead she can't do anything
    if (currentState == State.DEAD or currentState == State.WORM) then
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
            footstepsParticle:GetTransform():SetPosition(float3.new(componentTransform:GetPosition().x,
                componentTransform:GetPosition().y + 1, componentTransform:GetPosition().z))
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
    impactParticle:GetComponentParticle():SetLoop(true)
    impactParticle:GetTransform():SetPosition(float3.new(target:GetTransform():GetPosition().x,
        target:GetTransform():GetPosition().y + 17, target:GetTransform():GetPosition().z + 5))

    LookAtTarget(target:GetTransform():GetPosition())
end

function DoAttack()

    componentAnimator:SetSelectedClip("AttackToIdle")

    DispatchGlobalEvent("Player_Attack", {target, characterID})
    DispatchGlobalEvent("Auditory_Trigger", {componentTransform:GetPosition(), 100, "single", gameObject})

    LookAtTarget(target:GetTransform():GetPosition())

    trackList = {4, 8}
    ChangeTrack(trackList)

    attackTimer = 0.0

    target = nil
    impactParticle:GetComponentParticle():SetLoop(false)

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
        end
    end
end

function CastPrimary(position)
    abilities.AbilityPrimary = AbilityStatus.Using
    DispatchGlobalEvent("Player_Ability", {characterID, Ability.Primary, abilities.AbilityPrimary})

    componentAnimator:SetSelectedClip("Dart")
    StopMovement(false)

    LookAtTarget(position)
end

function FireDart()

    InstantiatePrefab("Dart")

    primaryTimer = 0.0
    abilities.AbilityPrimary = AbilityStatus.Cooldown
    DispatchGlobalEvent("Player_Ability", {characterID, Ability.Primary, abilities.AbilityPrimary, primaryCooldown})

    trackList = {5, 9}
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

function CastSecondary(position)
    abilities.AbilitySecondary = AbilityStatus.Using
    DispatchGlobalEvent("Player_Ability", {characterID, Ability.Secondary, abilities.AbilitySecondary})

    componentAnimator:SetSelectedClip("Smokebomb")
    StopMovement(false)

    LookAtTarget(position)
end

function PlaceSmokebomb()

    InstantiatePrefab("Smokebomb")
    smokeBombCount = smokeBombCount - 1

    -- secondaryTimer = 0.0

    trackList = {6}
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

function CastUltimate(position)
    abilities.AbilityUltimate = AbilityStatus.Using
    DispatchGlobalEvent("Player_Ability", {characterID, Ability.Ultimate, abilities.AbilityUltimate})

    componentAnimator:SetSelectedClip("Mosquito")

    StopMovement(false)

    LookAtTarget(position)

    trackList = {7}
    ChangeTrack(trackList)
end

function DoUltimate()
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

    if (currentHP > 1) then
        currentHP = currentHP - damage
        DispatchGlobalEvent("Player_Health", {characterID, currentHP, maxHP})

        trackList = {2}
        ChangeTrack(trackList)
    else
        currentHP = 0
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
        trackList = {10}
        ChangeTrack(trackList)
        ultimateTimer = 0.0
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
        trackList = {11}
        ChangeTrack(trackList)
    elseif (key == "Spit_Heal_Hit") then
        if (fields[1] == gameObject) then
            if (currentHP < maxHP) then
                currentHP = currentHP + fields[2]
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
    elseif (key == "Update_Nerala_Position") then
        Log("Receiving Nerala Position \n")
        componentRigidBody:SetRigidBodyPos(float3.new(fields[1], fields[2], fields[3]))
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

function ConfigStaminaBars()
    Log("Configuring stamina bars\n")
    staminaBarYellow = Find("Stamina Bar Yellow")
    staminaBarGreen = Find("Stamina Bar Green")
    staminaBarRed = Find("Stamina Bar Red")
    staminaBarBlue = Find("Stamina Bar Blue")

    staminaBarSizeX = staminaBarGreen:GetTransform():GetScale().x
    staminaBarSizeY = staminaBarGreen:GetTransform():GetScale().y
    staminaBarSizeZ = staminaBarGreen:GetTransform():GetScale().z
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

