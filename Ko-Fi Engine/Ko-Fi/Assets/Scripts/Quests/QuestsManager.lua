id = 1
local src = " "
local char = " "
local line1 = " "
local line2 = " "
local targetID = -1

local path = "Assets/Quests/quests.json"
local prefabPath = "Mission"
local missionIndex = 1

local missionCap = 5

missionList = {}

------------ Dialogue Manager ------------
function Start()
    --if (GetInput(14) == KEY_STATE.KEY_DOWN) then -- Q
        --limit the logic with the capMission
        --if (missionIndex < missionCap) then
           -- InstantiateNamedPrefab(prefabPath, "Mission_" .. missionIndex )
            --if(missionIndex > 1) then
            --local newMission = Find("Mission_" .. missionIndex - 1)
            --table.insert(missionList, newMission)

            --Layout()
            --end
            --missionIndex = missionIndex + 1
        --end
    --end
end

function Update(dt)

    -- TODO: SET THE ID FROM EVENT AND SET OPENDIALOGUE TO TRUE
    -- DispatchEvent(pathfinderUpdateKey, { patrolWaypoints, pingpong, componentTransform:GetPosition() })
    
    
    
end

function PostUpdate(dt)
    
end

function EventHandler(key, fields)
    
end
------------ END Dialogue Manager ------------

------------ Dialogue ------------

function CompleteQuest()
    
end

function AbandonQuest()
   
end


function Layout()
    for i = 1, #missionList do
        Log(missionList[i]:GetName() .. "\n")
        local mission = missionList[i]:GetChildren()[1]
        missionPosition = mission:GetTransform2D():GetPosition()
        missionPosition.y = missionPosition.y - (1 * 145)
        mission:GetTransform2D():SetPosition(float2.new(missionPosition.x, missionPosition.y))
    end
end




------------ END Dialogue ------------
print("Dialogue Script Load Success")
