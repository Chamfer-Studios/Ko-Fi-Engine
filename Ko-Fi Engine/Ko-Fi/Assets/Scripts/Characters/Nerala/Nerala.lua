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
    CROUCH = 4
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
speed = 2500.0

-- Basic Attack --
attackRange = 50.0
attackTime = 2.5

-- Primary ability --
primaryCastRange = 100
primaryCooldown = 5.0
drawPrimary = false

-- Secondary ability --
secondaryCastRange = 75
secondaryCooldown = 10.0
drawSecondary = false

-- Ultimate ability --
ultimateCastRange = 50
ultimateCooldown = 30.0
drawUltimate = false
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

-- speedIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT
-- speedIV = InspectorVariable.new("speed", speedIVT, speed)
-- NewVariable(speedIV)

-- Primary ability --
primaryCastRangeIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
primaryCastRangeIV = InspectorVariable.new("primaryCastRange", primaryCastRangeIVT, primaryCastRange)
NewVariable(primaryCastRangeIV)

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

    componentAnimator = gameObject:GetComponentAnimator()
    if (componentAnimator ~= nil) then
        componentAnimator:SetSelectedClip("Idle")
    else
        Log("[ERROR] Component Animator not found!\n")
    end

    mouseParticles = Find("Mouse Particles")
    if (mouseParticles ~= nil) then
        mouseParticles:GetComponentParticle():StopParticleSpawn()
    end

    componentRigidBody = gameObject:GetRigidBody()

    componentBoxCollider = gameObject:GetBoxCollider()

    componentSwitch = gameObject:GetAudioSwitch()
    currentTrackID = -1

    currentHP = maxHP
    DispatchGlobalEvent("Player_Health", {characterID, currentHP, maxHP})

    radiusLight = gameObject:GetLight()
end

-- Called each loop iteration
function Update(dt)

    DrawActiveAbilities()

    if (lastRotation ~= nil) then
        componentTransform:LookAt(lastRotation, float3.new(0, 1, 0))
    end

    if (ManageTimers(dt) == false) then
        return
    end

    -- States
    if (target ~= nil) then
        if (Distance3D(componentTransform:GetPosition(), target:GetTransform():GetPosition()) <= attackRange) then
            Attack()
        else
            destination = target:GetTransform():GetPosition()
            MoveToDestination(dt)
            DispatchEvent("Pathfinder_FollowPath", {speed, dt, false})
            DispatchGlobalEvent("Player_Position", {componentTransform:GetPosition(), gameObject})
        end
    elseif (destination ~= nil) then
        MoveToDestination(dt)
        DispatchEvent("Pathfinder_FollowPath", {speed, dt, false})
        DispatchGlobalEvent("Player_Position", {componentTransform:GetPosition(), gameObject})
    end

    -- Gather Inputs
    if (IsSelected() == true) then

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
                    else
                        if (Distance3D(target:GetTransform():GetPosition(), componentTransform:GetPosition()) <=
                            primaryCastRange) then
                            Log("[FAIL] Ability Primary: Ability out of range!\n")
                        else
                            if (componentAnimator ~= nil) then
                                CastPrimary(target:GetTransform():GetPosition())
                            end
                        end
                    end
                end

                -- Secondary ability (Smokebomb)
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

                -- Ultimate ability (mosquito)
            elseif (currentState == State.AIM_ULTIMATE) then
                if (ultimateTimer ~= nil) then
                    Log("[FAIL] Ability Ultimate: Ability in cooldown!\n")
                else
                    GetGameObjectHovered() -- GetGameObjectHovered updates the last mouse click
                    local mouse = GetLastMouseClick()
                    if (Distance3D(mouse, componentTransform:GetPosition()) > ultimateCastRange) then
                        Log("[FAIL] Ability Ultimate: Ability out of range!\n")
                    else
                        target = mouse
                        if (componentAnimator ~= nil) then
                            CastUltimate(mouse)
                        end
                    end
                end

            end
        end

        -- Right Click
        if (GetInput(3) == KEY_STATE.KEY_DOWN) then
            goHit = GetGameObjectHovered()
            if (goHit ~= gameObject) then
                if (currentState == State.AIM_PRIMARY or currentState == State.AIM_SECONDARY or currentState ==
                    State.AIM_ULTIMATE) then
                    CancelAbilities()
                else
                    if (goHit.tag == Tag.ENEMY and
                        Distance3D(componentTransform:GetPosition(), goHit:GetTransform():GetPosition()) <= attackRange) then
                        target = goHit
                        Attack()
                    else
                        if (goHit.tag == Tag.ENEMY) then
                            target = goHit
                            destination = target:GetTransform():GetPosition()
                            DispatchEvent("Pathfinder_UpdatePath",
                                {{destination}, false, componentTransform:GetPosition()})
                        else
                            destination = GetLastMouseClick()
                            DispatchEvent("Pathfinder_UpdatePath",
                                {{destination}, false, componentTransform:GetPosition()})
                        end

                        if (currentMovement == Movement.WALK and isDoubleClicking == true) then

                            currentMovement = Movement.RUN

                            if (componentSwitch ~= nil) then
                                if (currentTrackID ~= -1) then
                                    componentSwitch:StopTrack(currentTrackID)
                                end
                                currentTrackID = 1
                                componentSwitch:PlayTrack(currentTrackID)
                            end
                        else
                            if (currentMovement == Movement.IDLE) then

                                currentMovement = Movement.WALK

                                if (componentSwitch ~= nil) then
                                    if (currentTrackID ~= -1) then
                                        componentSwitch:StopTrack(currentTrackID)
                                    end
                                    currentTrackID = 0
                                    componentSwitch:PlayTrack(currentTrackID)
                                end
                            end
                            isDoubleClicking = true
                        end
                        if (mouseParticles ~= nil) then
                            mouseParticles:GetComponentParticle():ResumeParticleSpawn()
                            mouseParticles:GetTransform():SetPosition(destination)
                        end
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
                currentState = State.AIM_PRIMARY
                DispatchGlobalEvent("Player_Ability", {characterID, 1, 1})
                drawPrimary = true
                drawSecondary = false
                drawUltimate = false
            end
        end

        -- 2
        if (GetInput(22) == KEY_STATE.KEY_DOWN) then
            if (currentState == State.AIM_SECONDARY) then
                CancelAbilities()
            else
                CancelAbilities()
                currentState = State.AIM_SECONDARY
                DispatchGlobalEvent("Player_Ability", {characterID, 2, 1})
                drawPrimary = false
                drawSecondary = true
                drawUltimate = false
            end
        end

        -- 3
        if (GetInput(23) == KEY_STATE.KEY_DOWN) then
            if (currentState == State.AIM_ULTIMATE) then
                CancelAbilities()
            else
                CancelAbilities()
                currentState = State.AIM_ULTIMATE
                DispatchGlobalEvent("Player_Ability", {characterID, 3, 1})
                drawPrimary = false
                drawSecondary = false
                drawUltimate = true
            end
        end

        -- LSHIFT -> Toggle crouch
        if (GetInput(12) == KEY_STATE.KEY_DOWN) then
            if (currentMovement == Movement.CROUCH) then
                if (destination ~= nil) then
                    currentMovement = Movement.WALK
                    if (componentSwitch ~= nil) then
                        if (currentTrackID ~= -1) then
                            componentSwitch:StopTrack(currentTrackID)
                        end
                        currentTrackID = 0
                        componentSwitch:PlayTrack(currentTrackID)
                    end
                else
                    currentMovement = Movement.IDLE
                end
            else
                if (currentMovement ~= Movement.IDLE and componentSwitch ~= nil) then
                    if (currentTrackID ~= -1) then
                        componentSwitch:StopTrack(currentTrackID)
                        currentTrackID = -1
                    end
                end
                currentMovement = Movement.CROUCH
            end
        end
    end
end
--------------------------------------------------

------------------- Functions --------------------
function CancelAbilities()
    currentState = State.IDLE
    DispatchGlobalEvent("Player_Ability", {characterID, 0, 0})
    drawPrimary = false
    drawSecondary = false
    drawUltimate = false
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

function ManageTimers(dt)
    local ret = true

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
    if (primaryTimer ~= nil) then

        primaryTimer = primaryTimer + dt
        if (primaryTimer >= primaryCooldown) then
            primaryTimer = nil
            DispatchGlobalEvent("Player_Ability", {characterID, 2, 0})
        end

    end

    -- Secondary ability cooldown
    if (secondaryTimer ~= nil) then
        secondaryTimer = secondaryTimer + dt
        if (secondaryTimer >= secondaryCooldown) then
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

        if (componentAnimator ~= nil) then
            if (currentMovement == Movement.WALK) then
                componentAnimator:SetSelectedClip("Walk")
            elseif (currentMovement == Movement.CROUCH) then
                componentAnimator:SetSelectedClip("Crouch")
            elseif (currentMovement == Movement.RUN) then
                componentAnimator:SetSelectedClip("Run")
            end
        end

        -- Adapt speed
        local s = speed
        if (currentMovement == Movement.CROUCH) then
            s = speed * 0.66
        elseif (currentMovement == Movement.RUN) then
            s = speed * 1.5
        end

        -- Adapt speed on arrive
        if (d < 2) then
            s = s * 0.5
        end

        -- Movement
        vec2 = Normalize(vec2, d)
        if (componentRigidBody ~= nil) then
            -- componentRigidBody:SetLinearVelocity(float3.new(vec2[1] * s * dt, 0, vec2[2] * s * dt))
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

    if (componentSwitch ~= nil and currentTrackID ~= -1) then
        componentSwitch:StopTrack(currentTrackID)
        currentTrackID = -1
    end

    currentMovement = Movement.IDLE -- Stops aimings and all States

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

    currentState = State.ATTACK
    componentAnimator:SetSelectedClip("Attack")

    StopMovement(false)
    LookAtTarget(target:GetTransform():GetPosition())
end

function DoAttack()

    componentAnimator:SetSelectedClip("AttackToIdle")
    DispatchGlobalEvent("Player_Attack", {target, characterID})
    LookAtTarget(target:GetTransform():GetPosition())
    attackTimer = 0.0
    target = nil
    currentState = State.IDLE
end

-- Primary ability
function CastPrimary(position)

    componentAnimator:SetSelectedClip("Dart")
    primaryTimer = 0.0
    StopMovement()

    DispatchGlobalEvent("Player_Ability", {characterID, 1, 2})
    LookAtTarget(position)
end

function FireDart()

    InstantiatePrefab("Dart")
    if (componentSwitch ~= nil) then
        if (currentTrackID ~= -1) then
            componentSwitch:StopTrack(currentTrackID)
        end
        currentTrackID = 2
        componentSwitch:PlayTrack(currentTrackID)
    end

    componentAnimator:SetSelectedClip("DartToIdle")
    currentState = State.IDLE
end

-- Secondary ability
function CastSecondary(position)

    componentAnimator:SetSelectedClip("Smokebomb")
    secondaryTimer = 0.0
    StopMovement()

    DispatchGlobalEvent("Player_Ability", {characterID, 2, 2})
    LookAtTarget(position)
end

function PlaceSmokebomb()

    InstantiatePrefab("Smokebomb")
    if (componentSwitch ~= nil) then
        if (currentTrackID ~= -1) then
            componentSwitch:StopTrack(currentTrackID)
        end
        currentTrackID = 3
        componentSwitch:PlayTrack(currentTrackID)
    end

    componentAnimator:SetSelectedClip("SmokebombToIdle")
    currentState = State.IDLE
end

-- Ultimate ability
function CastUltimate(position)

    componentAnimator:SetSelectedClip("Mosquito")
    -- CD will start when the mosquito dies		
    StopMovement()

    DispatchGlobalEvent("Player_Ability", {characterID, 3, 2})
    LookAtTarget(position)
end

function DoUltimate()

    InstantiatePrefab("Mosquito")

    -- No new clip, the last clip has to last until the mosquito dies
    currentState = State.MOSQUITO
end

function TakeDamage(damage)
    if (damage == nil) then
        damage = 1
    end

    iFramesTimer = 0.0

    if (currentHP > 1) then
        currentHP = currentHP - damage
        DispatchGlobalEvent("Player_Health", {characterID, currentHP, maxHP})

        if (componentSwitch ~= nil) then
            if (currentTrackID ~= -1) then
                componentSwitch:StopTrack(currentTrackID)
            end
            currentTrackID = 4 -- Should be 5
            componentSwitch:PlayTrack(currentTrackID)
        end
    else
        Die()
    end
end

function Die()

    StopMovement()

    currentState = State.DEAD
    currentHP = 0
    DispatchGlobalEvent("Player_Health", {characterID, currentHP, maxHP})

    if (componentAnimator ~= nil) then
        componentAnimator:SetSelectedClip("Death")
    end
    if (componentSwitch ~= nil) then
        if (currentTrackID ~= -1) then
            componentSwitch:StopTrack(currentTrackID)
        end
        currentTrackID = 5 -- Should be 6
        componentSwitch:PlayTrack(currentTrackID)
    end
end
--------------------------------------------------

-------------------- Events ----------------------
function EventHandler(key, fields)

    if (key == "Mosquito_Death") then
        ultimateTimer = 0.0
        currentState = State.IDLE
    end
end
--------------------------------------------------

------------------ Collisions --------------------
function OnTriggerEnter(go)
    if (go.tag == Tag.ENEMY and iFramesTimer == nil) then
        TakeDamage(1)
    end
end

function OnCollisionEnter(go)

    if (go.tag == Tag.ENEMY and iFramesTimer == nil) then
        TakeDamage(1)
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

print("Nerala.lua compiled succesfully")

-------- Scraps --------
-- local components = gameObject:GetComponents()
-- print(components[3].type) -- return it as an int, can't associate back to enum (I don't know how to anyway)

-- GameState = require "Assets.Scripts.GameState"
-- GameState:Update(1)
-- print(GameState:GetGameState())
