id = 1
local src = " "
local char = " "
local line1 = " "
local line2 = " "
local targetID = -1

local path = "Assets/Dialogues/dialogues.json"
local prefabPath= "Dialogue"

openDialogue = false

------------ Dialogue Manager ------------
function Start()
	LoadJsonFile(path)
	
end

function Update(dt)
	-- TODO: SET THE ID FROM EVENT AND SET OPENDIALOGUE TO TRUE
	--DispatchEvent(pathfinderUpdateKey, { patrolWaypoints, pingpong, componentTransform:GetPosition() })
	if(openDialogue == true) then
		if(Find("DialogueSkipButton")) then
			OpenDialogue()
		end
	end

	CheckIfSkipped()

end

function EventHandler(key, fields)
    if (key == "DialogueTriggered") then -- fields[1] -> go;
		id = fields[1]
		InstantiatePrefab(prefabPath) -- Create Dialogue Prefab
		openDialogue = true
    end
end
------------ END Dialogue Manager ------------


------------ Dialogue ------------

function OpenDialogue()
	--print("open dialogue")
	SetDialogueValues()
	openDialogue = false
	
end

function CloseDialogue()
	
	DeleteGameObjectByUID(Find("Dialogue"):GetUID())
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
	char = GetDialogueString("char",id)
	line1 = GetDialogueString("line1", id)
	line2 = GetDialogueString("line2", id)
	targetID = GetDialogueTargetID("targetID", id)

	-- Set Values To The Prefab
	Find("DialogueAvatar"):GetImage():SetTexture(src)
	Find("DialogueName"):GetText():SetTextValue(char)
	Find("DialogueLine1"):GetText():SetTextValue(line1)
	Find("DialogueLine2"):GetText():SetTextValue(line2)

end

function CheckIfSkipped()
	-- Check Next Dialogue or Close Dialogue
	-- If Button Does not exist Dont execute dialogue
	if(Find("DialogueSkipButton")) then

		skipButton = Find("DialogueSkipButton"):GetButton():IsPressed()
		if (skipButton == true) then

			if(targetID ~= -1) then -- Next Dialogue
				id = targetID
				SetDialogueValues()	
			else
				CloseDialogue() -- Close Dialogue
			end
		end
	end
end

------------ END Dialogue ------------
print("Dialogue Script Load Success")
