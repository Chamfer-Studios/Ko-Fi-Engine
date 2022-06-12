id = 1
local src = " "
local char = " "
local line1 = " "
local line2 = " "
local targetID = -1

local path = "Assets/Dialogues/dialogues.json"
local prefabPath = "Dialogue"
local characterUnlockPrefab = "UnlockPrefab"

local isUnlocking = false;
local unlockedCharId = -1;

openDialogue = false
openFeedback = false

------------ Dialogue Manager ------------
function Start()
    LoadJsonFile(path)

end

function Update(dt)
    -- TODO: SET THE ID FROM EVENT AND SET OPENDIALOGUE TO TRUE
    -- DispatchEvent(pathfinderUpdateKey, { patrolWaypoints, pingpong, componentTransform:GetPosition() })
    if (openDialogue == true) then
        if (Find("DialogueSkipButton")) then
            OpenDialogue()
        end
    end
    if (openFeedback == true) then
        if (Find("UnlockFeedbackAvatar")) then
            OpenFeedback()
        end

    end

    CheckIfSkipped()

end

function EventHandler(key, fields)
    if (key == "DialogueTriggered") then -- fields[1] -> go;
        id = fields[1]
        InstantiatePrefab(prefabPath) -- Create Dialogue Prefab
        openDialogue = true
        isUnlocking = false
        if fields[2] ~= nil then
            isUnlocking = true
            unlockedCharId = fields[2]
        end
    end
end
------------ END Dialogue Manager ------------

------------ Dialogue ------------

function OpenDialogue()
    -- print("open dialogue")
    SetDialogueValues()
    openDialogue = false
    DispatchGlobalEvent("Dialogue_Opened", {})
end

function OpenFeedback()
    -- print("open dialogue")
    SetUnlockFeedbackValues()
    openFeedback = false
end

function CloseDialogue()

    DeleteGameObjectByUID(Find("Dialogue"):GetUID())
    if (isUnlocking) then
        Log("Unlocking character " .. unlockedCharId .. "\n")
        -- InstantiatePrefab(characterUnlockPrefab) -- Create Char Unlock Feedback Prefab
        -- openFeedback = true;
        DispatchGlobalEvent("Enable_Character", {unlockedCharId})
        isUnlocking = false;
    end
    src = ""
    char = ""
    line1 = ""
    line2 = ""
    targetID = -1
    id = -1
end

function SetDialogueValues()
    -- Get Dialogue Values From JSON
    -- print("setting Values")
    src = GetDialogueString("src", id)
    char = GetDialogueString("char", id)
    line1 = GetDialogueString("line1", id)
    line2 = GetDialogueString("line2", id)
    targetID = GetDialogueTargetID("targetID", id)

    -- Set Values To The Prefab
    Find("DialogueAvatar"):GetImage():SetTexture(src)
    Find("DialogueName"):GetText():SetTextValue(char)
    Find("DialogueLine1"):GetText():SetTextValue(line1)
    Find("DialogueLine2"):GetText():SetTextValue(line2)

end

function SetUnlockFeedbackValues()
    -- Get Dialogue Values From JSON
    -- print("settin Values")
    char = GetDialogueString("char", id)
    srcUnlock = GetDialogueString("srcUnlock", id)
    -- Set Values To The Prefab

    Find("UnlockFeedbackAvatar"):GetImage():SetTexture(srcUnlock)
    Find("UnlockText"):GetText():SetTextValue(char .. " Unlocked")

    src = ""
    char = ""
    line1 = ""
    line2 = ""
    targetID = -1
    id = -1

end

function CheckIfSkipped()
    -- Check Next Dialogue or Close Dialogue
    -- If Button Does not exist Dont execute dialogue
    if (Find("DialogueSkipButton")) then

        skipButton = Find("DialogueSkipButton"):GetButton():IsPressed()
        if (skipButton == true) then

            if (targetID ~= -1) then -- Next Dialogue
                id = targetID
                SetDialogueValues()
            else
                CloseDialogue() -- Close Dialogue
                DispatchGlobalEvent("Dialogue_Closed", {})
            end
        end
    end
end

------------ END Dialogue ------------
print("Dialogue Script Load Success")
