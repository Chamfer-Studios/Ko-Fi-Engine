id = 1
local src = " "
local char = " "
local line1 = " "
local line2 = " "
local targetID = -1

local path = "Assets/Quests/quests.json"
local prefabPath = "Mission"
local missionIndex = 1

missionList = {}

------------ Dialogue Manager ------------
function Start()

end

function Update(dt)
    -- TODO: SET THE ID FROM EVENT AND SET OPENDIALOGUE TO TRUE
    -- DispatchEvent(pathfinderUpdateKey, { patrolWaypoints, pingpong, componentTransform:GetPosition() })
    if (GetInput(14) == KEY_STATE.KEY_DOWN) then -- Q
        InstantiateNamedPrefab(prefabPath, "Mission_" .. missionIndex )
        table.insert(missionList, Find("Mission_" .. missionIndex))
        missionIndex = missionIndex + 1
        Layout()
    end
    
    
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
        Log("Mission Name:" .. missionList[i]:GetName() .. "\n")
    end
end




------------ END Dialogue ------------
print("Dialogue Script Load Success")
