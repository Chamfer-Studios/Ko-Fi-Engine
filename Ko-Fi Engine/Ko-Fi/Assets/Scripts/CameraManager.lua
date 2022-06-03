rotSpeed = 150.0
panSpeed = 150.0
zoomSpeed = 50
finalPos = float3.new(0, 0, 0)
angle = 45.0
remainingAngle = 0.0
offset = float3.new(0, 240, 270)
scrollspeed = 15.0
targetAngle = 0.0
newZoomedPos = float3.new(0, 0, 0)
mosquitoAlive = false
zPanning = 0.0 -- 1 to go front, -1 to go back
xPanning = 0.0 -- 1 to go right, -1 to go left
--resetOffset = 1;
--currentTarget = float3.new(0, 0, 0)
local freePanning = true
local closestY = 100.0
local furthestY = 2000.0

--use the fokin start
function Start()
    --Put the position of the selected character inside variable target
    GetSelectedCharacter()
end

function Update(dt)

    --input: mouse wheel to zoom in and out
    -- local?
    if (GetMouseZ() > 0) then
        if finalPos.y < furthestY then
            newZoomedPos.y = newZoomedPos.y + gameObject:GetCamera():GetFront().y * zoomSpeed
            newZoomedPos.x = newZoomedPos.x + gameObject:GetCamera():GetFront().x * zoomSpeed
            newZoomedPos.z = newZoomedPos.z + gameObject:GetCamera():GetFront().z * zoomSpeed
        end
    elseif (GetMouseZ() < 0) then
        if finalPos.y > closestY then
            newZoomedPos.y = newZoomedPos.y - gameObject:GetCamera():GetFront().y * zoomSpeed
            newZoomedPos.x = newZoomedPos.x - gameObject:GetCamera():GetFront().x * zoomSpeed
            newZoomedPos.z = newZoomedPos.z - gameObject:GetCamera():GetFront().z * zoomSpeed
        end
    end

    --input: wasd keys to pan the camera freely
    --i had to do both key up and down. Down is for activating the panning in the propper direction
    --and up resets it to zero
    if (GetInput(16) == KEY_STATE.KEY_DOWN) then -- W --HAY UN KEY REPEAT
        zPanning = -1.0
    end
    if (GetInput(16) == KEY_STATE.KEY_UP) then -- W
        zPanning = 0.0
    end
    if (GetInput(17) == KEY_STATE.KEY_DOWN) then -- A
        xPanning = -1.0
    end
    if (GetInput(17) == KEY_STATE.KEY_UP) then -- A
        xPanning = 0.0
    end
    if (GetInput(18) == KEY_STATE.KEY_DOWN) then -- S
        zPanning = 1.0
    end
    if (GetInput(18) == KEY_STATE.KEY_UP) then -- S
        zPanning = 0.0
    end
    if (GetInput(19) == KEY_STATE.KEY_DOWN) then -- D
        xPanning = 1.0
    end
    if (GetInput(19) == KEY_STATE.KEY_UP) then -- D
        xPanning = 0.0
    end

    -- go back to the selected character
    if  (GetInput(10) == KEY_STATE.KEY_DOWN) then -- R
        offset = float3.new(0, 240, 270)
        GetSelectedCharacter()
    end

    if  (GetInput(14) == KEY_STATE.KEY_REPEAT) then -- Q
        local newQuat = Quat.new(float3.new(0, 1, 0), -0.0174533)
        offset = MulQuat(newQuat, offset) 
    end
    if  (GetInput(15) == KEY_STATE.KEY_REPEAT) then -- E
        local newQuat = Quat.new(float3.new(0, 1, 0), 0.0174533)
        offset = MulQuat(newQuat, offset)
    end

    --Log("panning " .. xPanning .. " " .. zPanning .. "\n")


    --input: click mouse wheel to orbit the camera
    --TODO

    local currentPanSpeed = panSpeed * dt
    
    -- modify target with the camera panning
    if (freePanning == true) then
        target.z = target.z + (zPanning * currentPanSpeed)
        target.x = target.x + (xPanning * currentPanSpeed)
    end

    --Log("current target " .. target.x .. " " .. target.z .. "\n")

    --add offset to the target to set the current camera position
    local newPos = float3.new(0, 0, 0)
    newPos.x = target.x + offset.x
    newPos.y = target.y + offset.y
    newPos.z = target.z + offset.z
  
    -- --compute final position adding zoom
    finalPos.x = newPos.x + newZoomedPos.x 
    finalPos.y = newPos.y + newZoomedPos.y
    finalPos.z = newPos.z + newZoomedPos.z

    componentTransform:SetPosition(finalPos)

    gameObject:GetCamera():LookAt(target)
    --1st iteration use look at to center at characters
    
end

function EventHandler(key, fields) --funcion virtual que recibe todos los eventos que se componen de una key y unos fields. la cosa esta en especificar la key (quees el evento) 
    if (key == "Mosquito_Spawn") then
        mosquito = fields[1]
    elseif (key == "Mosquito_Death") then
        mosquito = nil
    end
end

function GetSelectedCharacter()

    --get character selected. I keep it in order to center the camera arround a player in case needed
    id = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
    if (id == 1) then
        target = Find("Zhib"):GetTransform():GetPosition()
        lastTarget = id
    elseif (id == 2) then
        if (mosquito ~= nil) then
            target = mosquito:GetTransform():GetPosition()
        else
            target = Find("Nerala"):GetTransform():GetPosition()
        end
        lastTarget = id
    elseif (id == 3) then
        target = Find("Omozra"):GetTransform():GetPosition()
        lastTarget = id
    else
        if (lastTarget == 1) then
            target = Find("Zhib"):GetTransform():GetPosition()
        elseif (lastTarget == 2) then
            if (mosquitoAlive == true and mosquito ~= nil) then
                target = mosquito:GetTransform():GetPosition()
            else
                target = Find("Nerala"):GetTransform():GetPosition()
            end
        elseif (lastTarget == 3) then
            target = Find("Omozra"):GetTransform():GetPosition()
        end
    end

end
