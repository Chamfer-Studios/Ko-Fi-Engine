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
    DEAD = 6,
    WORM = 7
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
characterID = 1
speed = 2000
crouchMultiplierPercentage = 66
runMultiplierPercentage = 150
staminaSeconds = 5
recoveryTime = 3
staminaTimer = staminaSeconds
isTired = false

-- Basic Attack --
attackRange = 25.0
attackTime = 2.5

-- Primary ability --
primaryCastRange = 100
maxKnives = 2
knifeSpeed = 3000
unawareChanceHarkKnife = 100
awareChanceHarkKnife = 80
aggroChanceHarkKnife = 20
unawareChanceSardKnife = 75
awareChanceSardKnife = 25
aggroChanceSardKnife = 0
drawPrimary = false

-- Secondary ability --
secondaryCastRange = 75
secondaryCooldown = 10.0
drawSecondary = false

-- Ultimate ability --
ultimateCastRange = 50
ultimateCooldown = 30.0
drawUltimate = false
ultimateCastRangeExtension = ultimateCastRange * 0.5
---------------------------------------------------------

-------------------- Movement logic ---------------------
doubleClickDuration = 0.5
doubleClickTimer = 0.0
isDoubleClicking = false
---------------------------------------------------------

------------------- Inspector setter --------------------
-- Globals --
maxHPIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
maxHPIV = InspectorVariable.new("maxHP", maxHPIVT, maxHP)
NewVariable(maxHPIV)

speedIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
speedIV = InspectorVariable.new("speed", speedIVT, speed)
NewVariable(speedIV)

crouchMultiplierPercentageIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
crouchMultiplierPercentageIV = InspectorVariable.new("crouchMultiplierPercentage", crouchMultiplierPercentageIVT,
    crouchMultiplierPercentage)
NewVariable(crouchMultiplierPercentageIV)

runMultiplierPercentageIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
runMultiplierPercentageIV = InspectorVariable.new("runMultiplierPercentage", runMultiplierPercentageIVT,
    runMultiplierPercentage)
NewVariable(runMultiplierPercentageIV)

staminaSecondsIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
staminaSecondsIV = InspectorVariable.new("staminaSeconds", staminaSecondsIVT, staminaSeconds)
NewVariable(staminaSecondsIV)

recoveryTimeIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
recoveryTimeIV = InspectorVariable.new("recoveryTime", recoveryTimeIVT, recoveryTime)
NewVariable(recoveryTimeIV)

---- Primary ability --
primaryCastRangeIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
primaryCastRangeIV = InspectorVariable.new("primaryCastRange", primaryCastRangeIVT, primaryCastRange)
NewVariable(primaryCastRangeIV)

maxKnivesIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
maxKnivesIV = InspectorVariable.new("maxKnives", maxKnivesIVT, maxKnives)
NewVariable(maxKnivesIV)

knifeSpeedIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
knifeSpeedIV = InspectorVariable.new("knifeSpeed", knifeSpeedIVT, knifeSpeed)
NewVariable(knifeSpeedIV)

---- Secondary ability --
secondaryCastRangeIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
secondaryCastRangeIV = InspectorVariable.new("secondaryCastRange", secondaryCastRangeIVT, secondaryCastRange)
NewVariable(secondaryCastRangeIV)

---- Ultimate ability --
ultimateCastRangeIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
ultimateCastRangeIV = InspectorVariable.new("ultimateCastRange", ultimateCastRangeIVT, ultimateCastRange)
NewVariable(ultimateCastRangeIV)
---------------------------------------------------------

----------------------- Methods -------------------------

function Start()

    componentAnimator = gameObject:GetParent():GetComponentAnimator()
    if (componentAnimator ~= nil) then
        componentAnimator:SetSelectedClip("Idle")
    else
        Log("[ERROR] Component Animator not found!\n")
    end

    mouseParticles = Find("Mouse Particles")
    if (mouseParticles ~= nil) then
        mouseParticles:GetComponentParticle():StopParticleSpawn()
    end
    choosingTargetParticle = Find("Choosing Target")

    componentRigidBody = gameObject:GetRigidBody()

    componentBoxCollider = gameObject:GetBoxCollider()

    componentSwitch = gameObject:GetAudioSwitch()
    currentTrackID = -1

    radiusLight = gameObject:GetLight()

    currentHP = maxHP

    knifeCount = maxKnives

    DispatchGlobalEvent("Player_Health", {characterID, currentHP, maxHP})

    characterSelectedMesh = Find("CharacterSelectedMesh")
    staminaBarSizeY = characterSelectedMesh:GetTransform():GetScale().y
end

-- Called each loop iteration
function Update(dt)
    DrawActiveAbilities()
    DrawHoverParticle()

    DispatchGlobalEvent("Player_Position", {componentTransform:GetPosition(), gameObject})

    if (knifeCount == 1) then

    end

    if (lastRotation ~= nil) then
        componentTransform:LookAt(lastRotation, float3.new(0, 1, 0))
    end

    if (ManageTimers(dt) == false) then
        return
    end

    -- States
    if (currentState == State.ATTACK) then

        if (Distance3D(componentTransform:GetPosition(), target:GetTransform():GetPosition()) <= attackRange) then
            Attack()
        else
            destination = target:GetTransform():GetPosition()
            MoveToDestination(dt)
        end
    elseif (currentState == State.AIM_PRIMARY or currentState == State.AIM_SECONDARY or currentState ==
        State.AIM_ULTIMATE) then
        StopMovement()
        componentAnimator:SetSelectedClip("Idle")
    elseif (destination ~= nil) then
        MoveToDestination(dt)
    end

    -- Gather Inputs
    if (IsSelected() == true) then

        UpdateStaminaBar()

        -- Left Click
        if (GetInput(1) == KEY_STATE.KEY_DOWN) then

            -- Primary ability (Knife)
            if (currentState == State.AIM_PRIMARY) then
                if (knifeCount <= 0) then
                    Log("[FAIL] Ability Primary: You don't have enough knives!\n")
                else
                    target = GetGameObjectHovered()
                    if (target.tag ~= Tag.ENEMY) then
                        Log("[FAIL] Ability Primary: You have to select an enemy first!\n")
                    else
                        if (Distance3D(target:GetTransform():GetPosition(), componentTransform:GetPosition()) >
                            primaryCastRange) then
                            Log("[FAIL] Ability Primary: Ability out of range!\n")
                        else
                            if (componentAnimator ~= nil) then
                                CastPrimary(target:GetTransform():GetPosition())
                            end
                        end
                    end
                end

                -- Secondary ability (Decoy)
            elseif (currentState == State.AIM_SECONDARY) then
                if (secondaryTimer ~= nil) then
                    Log("[FAIL] Ability Secondary: Ability in cooldown!\n")
                else
                    GetGameObjectHovered() -- GetGameObjectHovered updates the last mouse click
                    local mouse = GetLastMouseClick()
                    if (Distance3D(mouse, componentTransform:GetPosition()) > secondaryCastRange) then
                        Log("[FAIL] Ability Secondary: Ability out of range!\n")
                    else
                        target = mouse
                        if (componentAnimator ~= nil) then
                            CastSecondary(mouse)
                        end
                    end
                end

                -- Ultimate ability (master yi)
            elseif (currentState == State.AIM_ULTIMATE) then
                if (ultimateTimer ~= nil) then
                    Log("[FAIL] Ability Ultimate: Ability in cooldown!\n")
                else
                    target = GetGameObjectHovered()
                    if (target.tag ~= Tag.ENEMY) then
                        Log("[FAIL] Ability Ultimate: You have to select an enemy first!\n")
                    else
                        if (Distance3D(target:GetTransform():GetPosition(), componentTransform:GetPosition()) >
                            ultimateCastRange) then
                            Log("[FAIL] Ability Ultimate: Ability out of range!\n")
                        else
                            if (componentAnimator ~= nil) then
                                CastUltimate(target:GetTransform():GetPosition())
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
                    local isMoving = true
                    if (goHit.tag == Tag.ENEMY) then
                        SetState(State.ATTACK)
                        target = goHit
                        if (Distance3D(componentTransform:GetPosition(), goHit:GetTransform():GetPosition()) <=
                            attackRange) then
                            isMoving = false
                            Attack()
                        else
                            destination = target:GetTransform():GetPosition()
                            DispatchEvent("Pathfinder_UpdatePath",
                                {{destination}, false, componentTransform:GetPosition()})
                        end
                    elseif (Distance3D(componentTransform:GetPosition(), GetLastMouseClick()) > 10) then
                        destination = GetLastMouseClick()
                        DispatchEvent("Pathfinder_UpdatePath", {{destination}, false, componentTransform:GetPosition()})
                    else
                        isMoving = false
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
                    if (mouseParticles ~= nil) then
                        mouseParticles:GetComponentParticle():SetLoop(true)
                        mouseParticles:GetComponentParticle():ResumeParticleSpawn()
                        mouseParticles:GetTransform():SetPosition(destination)
                    end
                end
            end
        end

        -- 1
        if (GetInput(21) == KEY_STATE.KEY_DOWN) then

            if (currentState == State.AIM_PRIMARY) then
                CancelAbilities()
            else
                CancelAbilities()
                SetState(State.AIM_PRIMARY)
                DispatchGlobalEvent("Player_Ability", {characterID, 1, 1})
                drawPrimary = true
            end
        end

        -- 2
        if (GetInput(22) == KEY_STATE.KEY_DOWN) then
            if (currentState == State.AIM_SECONDARY) then
                CancelAbilities()
            else
                CancelAbilities()
                SetState(State.AIM_SECONDARY)
                DispatchGlobalEvent("Player_Ability", {characterID, 2, 1})
                drawSecondary = true
            end
        end

        -- 3
        if (GetInput(23) == KEY_STATE.KEY_DOWN) then
            if (currentState == State.AIM_ULTIMATE) then
                CancelAbilities()
            else
                CancelAbilities()
                SetState(State.AIM_ULTIMATE)
                DispatchGlobalEvent("Player_Ability", {characterID, 3, 1})
                drawUltimate = true
            end
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
        CancelAbilities()
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
        if (componentSwitch ~= nil) then
            if (currentTrackID ~= -1) then
                componentSwitch:StopTrack(currentTrackID)
            end
            currentTrackID = 0
            componentSwitch:PlayTrack(currentTrackID)
        end
    elseif (newMovement == Movement.RUN) then
        currentMovement = Movement.RUN
        if (componentAnimator ~= nil) then
            componentAnimator:SetSelectedClip("Run")
        end
        if (componentSwitch ~= nil) then
            if (currentTrackID ~= -1) then
                componentSwitch:StopTrack(currentTrackID)
            end
            currentTrackID = 1
            componentSwitch:PlayTrack(currentTrackID)
        end
    elseif (newMovement == Movement.IDLE_CROUCH) then
        currentMovement = Movement.IDLE_CROUCH
        if (componentAnimator ~= nil) then
            componentAnimator:SetSelectedClip("IdleCrouch")
        end
        if (currentTrackID ~= -1) then
            componentSwitch:StopTrack(currentTrackID)
            currentTrackID = -1
        end
    elseif (newMovement == Movement.CROUCH) then
        currentMovement = Movement.CROUCH
        if (currentMovement ~= Movement.IDLE and componentSwitch ~= nil) then
            if (currentTrackID ~= -1) then
                componentSwitch:StopTrack(currentTrackID)
            end
            currentTrackID = 0
            componentSwitch:PlayTrack(currentTrackID)
        end
        if (componentAnimator ~= nil) then
            componentAnimator:SetSelectedClip("Crouch")
        end
    end
end

function CancelAbilities()
    if (currentState ~= State.WORM) then
        SetState(State.IDLE)
    end
    DispatchGlobalEvent("Player_Ability", {characterID, 0, 0})
    drawPrimary = false
    drawSecondary = false
    drawUltimate = false
end

function DrawHoverParticle()
    if (IsSelected() and
        (currentState == State.AIM_PRIMARY or currentState == State.AIM_SECONDARY or currentState == State.AIM_ULTIMATE)) then
        drawingTarget = GetGameObjectHovered
        if (drawingTarget.tag == Tag.ENEMY) then
            choosingTargetParticle:GetTransform():SetPosition(float3.new(playerPos.x, playerPos.y + 1, playerPos.z))
        end
    end
end

function DrawActiveAbilities()
    if radiusLight == nil then
        radiusLight = gameObject:GetLight()
    end
    if radiusLight ~= nil then
        if (drawPrimary == true) then
            radiusLight:SetRange(primaryCastRange)
            radiusLight:SetAngle(360 / 2)
        elseif (drawSecondary == true) then
            radiusLight:SetRange(secondaryCastRange)
            radiusLight:SetAngle(360 / 2)
        elseif (drawUltimate == true) then
            radiusLight:SetRange(ultimateCastRange)
            radiusLight:SetAngle(360 / 2)
        else
            radiusLight:SetAngle(0)
        end
    end
end

function UpdateStaminaBar()
    -- Log("Stamina proportion: " .. (staminaTimer / staminaSeconds) .. "\n")
    -- Log("Stamina final size: " .. staminaBarSizeY * (staminaTimer / staminaSeconds) .. "\n")
    characterSelectedMesh:GetTransform():SetScale(float3.new(characterSelectedMesh:GetTransform():GetScale().x,
        staminaBarSizeY * (staminaTimer / staminaSeconds), characterSelectedMesh:GetTransform():GetScale().z))
end

function ManageTimers(dt)
    local ret = true

    if (currentMovement == Movement.RUN) then
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
    -- if (knifePickupTimer ~= nil) then
    --     knifePickupTimer = knifePickupTimer + dt
    --     if (knifePickupTimer >= knifePickupTime) then
    --         knifePickupTimer = nil
    --     end
    -- end

    -- Secondary ability cooldown
    if (secondaryTimer ~= nil) then
        secondaryTimer = secondaryTimer + dt
        if (secondaryTimer >= secondaryCooldown) then
            Log("Decoy available!\n")
            secondaryTimer = nil
            DispatchGlobalEvent("Player_Ability", {characterID, 2, 0})
        end
    end

    -- Ultimate ability cooldown
    if (ultimateTimer ~= nil) then
        ultimateTimer = ultimateTimer + dt
        if (ultimateTimer >= ultimateCooldown) then
            ultimateTimer = nil
            DispatchGlobalEvent("Player_Ability", {characterID, 3, 0})
        end
    end

    -- Invisible logic
    if (invisibilityDuration ~= nil) then -- While invis he shouldn't be able to do anything
        invisibilityTimer = invisibilityTimer + dt
        if (invisibilityTimer >= invisibilityDuration) then
            -- Reappear
            invisibilityDuration = nil
            gameObject.active = true
            if (componentRigidBody ~= nil) then
                if (componentBoxCollider ~= nil) then
                    componentBoxCollider:SetTrigger(false)
                    componentBoxCollider:UpdateIsTrigger()
                end

                if (componentSwitch ~= nil) then
                    if (currentTrackID ~= -1) then
                        componentSwitch:StopTrack(currentTrackID)
                    end
                    currentTrackID = 7
                    componentSwitch:PlayTrack(currentTrackID)
                end

                componentRigidBody:SetUseGravity(true)
                componentRigidBody:UpdateEnableGravity()
            end
            if (componentAnimator ~= nil) then
                componentAnimator:SetSelectedClip("UltimateEnd")
            end
        end
        ret = false
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
                    FireKnife()
                elseif (currentState == State.AIM_SECONDARY) then
                    PlaceDecoy()
                elseif (currentState == State.AIM_ULTIMATE) then
                    DoUltimate()
                elseif (currentState ~= State.DEAD) then
                    componentAnimator:SetSelectedClip("Idle") -- Comment this line to test animations in-game
                end
            end
        end
    end

    -- If he's dead he can't do anything
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

-- Basic Attack
function Attack()

    SetState(State.ATTACK)

    componentAnimator:SetSelectedClip("Attack")

    LookAtTarget(target:GetTransform():GetPosition())
end

function DoAttack()

    componentAnimator:SetSelectedClip("AttackToIdle")

    DispatchGlobalEvent("Player_Attack", {target, characterID})
    DispatchGlobalEvent("Auditory_Trigger", {componentTransform:GetPosition(), 100, "single", gameObject})

    LookAtTarget(target:GetTransform():GetPosition())

    if (componentSwitch ~= nil) then
        if (currentTrackID ~= -1) then
            componentSwitch:StopTrack(currentTrackID)
        end
        currentTrackID = 4
        componentSwitch:PlayTrack(currentTrackID)
    end

    attackTimer = 0.0

    target = nil

    SetState(State.IDLE)
end

-- Primary ability
function CastPrimary(position)

    componentAnimator:SetSelectedClip("Knife")
    StopMovement(false)

    DispatchGlobalEvent("Player_Ability", {characterID, 1, 2})
    LookAtTarget(position)

    drawPrimary = false
    drawSecondary = false
    drawUltimate = false
end

function FireKnife()

    InstantiatePrefab("Knife")
    knifeCount = knifeCount - 1
    if (componentSwitch ~= nil) then
        if (currentTrackID ~= -1) then
            componentSwitch:StopTrack(currentTrackID)
        end
        currentTrackID = 5
        componentSwitch:PlayTrack(currentTrackID)
    end

    componentAnimator:SetSelectedClip("KnifeToIdle")
    SetState(State.IDLE)
end

-- Secondary ability
function CastSecondary(position)

    componentAnimator:SetSelectedClip("Decoy")
    StopMovement(false)

    DispatchGlobalEvent("Player_Ability", {characterID, 2, 2})
    LookAtTarget(position)

    drawPrimary = false
    drawSecondary = false
    drawUltimate = false
end

function PlaceDecoy()

    InstantiatePrefab("Decoy")

    if (componentSwitch ~= nil) then
        if (currentTrackID ~= -1) then
            componentSwitch:StopTrack(currentTrackID)
        end
        currentTrackID = 6
        componentSwitch:PlayTrack(currentTrackID)
    end

    componentAnimator:SetSelectedClip("DecoyToIdle")
    SetState(State.IDLE)
end

-- Ultimate ability
function CastUltimate(position)

    componentAnimator:SetSelectedClip("UltimateStart")
    ultimateTimer = 0.0
    StopMovement(false)

    DispatchGlobalEvent("Player_Ability", {characterID, 3, 2})
    LookAtTarget(position)

    drawPrimary = false
    drawSecondary = false
    drawUltimate = false
end

function DoUltimate()

    -- Get all enemies in range of the Mouse
    enemiesInRange = {target}
    enemies = GetObjectsByTag(Tag.ENEMY)
    for i = 1, #enemies do
        if (enemies[i] ~= target and
            Distance3D(enemies[i]:GetTransform():GetPosition(), target:GetTransform():GetPosition()) <=
            ultimateCastRange) then
            enemiesInRange[#enemiesInRange + 1] = enemies[i]
        end
    end

    -- If there are none, the ability isn't casted
    if (#enemiesInRange <= 0) then
        return
    end

    -- Check them all for adjacent enemies, different than the ones on the list and add them if there are anyway
    for i = 1, #enemiesInRange do

        for j = 1, #enemies do
            if (enemiesInRange[i] ~= enemies[j]) then
                if (Distance3D(enemiesInRange[i]:GetTransform():GetPosition(), enemies[j]:GetTransform():GetPosition()) <=
                    ultimateCastRangeExtension) then

                    isAlreadyInArray = false
                    for k = 1, #enemiesInRange do
                        if (enemiesInRange[k] == enemies[j]) then
                            isAlreadyInArray = true
                        end
                    end

                    if (not isAlreadyInArray) then
                        enemiesInRange[#enemiesInRange + 1] = enemies[j]
                    end
                end
            end
        end
    end

    deathMarkDuration = 0.3
    -- Set IN ORDER the death mark
    for i = 1, #enemiesInRange do
        DispatchGlobalEvent("Death_Mark", {enemiesInRange[i], deathMarkDuration})
        deathMarkDuration = deathMarkDuration + 0.3
    end

    -- final pos = final target pos + Normalized(final target pos - initial pos) * d
    local targetPos2D = {enemiesInRange[#enemiesInRange]:GetTransform():GetPosition().x,
                         enemiesInRange[#enemiesInRange]:GetTransform():GetPosition().z}
    local pos2D = {componentTransform:GetPosition().x, componentTransform:GetPosition().z}
    local d = Distance2D(pos2D, targetPos2D)
    local vec2 = {targetPos2D[1] - pos2D[1], targetPos2D[2] - pos2D[2]}
    vec2 = Normalize(vec2, d)

    -- Add as reappear position the position from the last enemy who's gonna die
    local dist = 15
    reappearPosition = float3.new(enemiesInRange[#enemiesInRange]:GetTransform():GetPosition().x + vec2[1] * dist,
        componentTransform:GetPosition().y,
        enemiesInRange[#enemiesInRange]:GetTransform():GetPosition().z + vec2[2] * dist)

    -- Set timer equal to the longest dath mark timer to reappear
    gameObject.active = false
    invisibilityTimer = 0
    invisibilityDuration = deathMarkDuration

    if (componentRigidBody ~= nil) then
        if (componentBoxCollider ~= nil) then
            componentBoxCollider:SetTrigger(true)
            componentBoxCollider:UpdateIsTrigger()
        end
        componentRigidBody:SetRigidBodyPos(reappearPosition)
        componentRigidBody:SetUseGravity(false)
        componentRigidBody:UpdateEnableGravity()
    end

    SetState(State.IDLE)
end

function TakeDamage(damage)
    if (iFramesTimer ~= nil or currentHP == 0) then
        return
    end

    iFramesTimer = 0

    if (damage == nil) then
        damage = 1
    end

    if (currentHP > 1) then
        currentHP = currentHP - damage
        DispatchGlobalEvent("Player_Health", {characterID, currentHP, maxHP})

        if (componentSwitch ~= nil) then
            if (currentTrackID ~= -1) then
                componentSwitch:StopTrack(currentTrackID)
            end
            currentTrackID = 2
            componentSwitch:PlayTrack(currentTrackID)
        end
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

    if (componentAnimator ~= nil) then
        componentAnimator:SetSelectedClip("Death")
    end
    if (componentSwitch ~= nil) then
        if (currentTrackID ~= -1) then
            componentSwitch:StopTrack(currentTrackID)
        end
        currentTrackID = 3
        componentSwitch:PlayTrack(currentTrackID)
    end
end
--------------------------------------------------

-------------------- Events ----------------------
function EventHandler(key, fields)
    if (key == "Stop_Movement") then
        StopMovement()
        if (componentAnimator ~= nil) then
            componentAnimator:SetSelectedClip("Idle")
        end
    elseif key == "Sadiq_Update_Target" then -- fields[1] -> target; targeted for (1 -> warning; 2 -> eat; 3 -> spit)

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
            end
        elseif (currentState == State.WORM and fields[2] == nil) then

            if (componentRigidBody ~= nil) then
                componentRigidBody:SetRigidBodyPos(fields[1])
            end
            gameObject.active = true
            SetState(State.IDLE)
        end
    elseif (key == "Decoy_Grabbed") then
        Log("I have grabbed the decoy! \n")
        secondaryTimer = 0.0
    elseif (key == "Knife_Grabbed") then
        Log("I have grabbed a knife! \n")
        knifeCount = knifeCount + 1
    elseif (key == "Enemy_Attack") then
        if (fields[1] == gameObject) then
            if (fields[2] == "Harkonnen") then
                TakeDamage()
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

print("Zhib.lua compiled succesfully")
Log("Zhib.lua compiled succesfully")
