id = 1
local idIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
idIV = InspectorVariable.new("id", idIVT, id)
NewVariable(idIV)

local src = " "
local char = " "
local line1 = " "
local line2 = " "
local targetID = -1

local path = "Assets/Dialogues/dialogues.json"
local prefabPath= "Dialogue"

local initialize = false

function Start()
	LoadJsonFile(path)
	InstantiatePrefab(prefabPath) -- Create Dialogue Prefab
end

function Update(dt)

	-- This is done this way because prefab is not loaded on Start
	if(Find("DialogueAvatar") ~= nil and initialize == false) then
		SetDialogueValues()
		initialize = true
	end

	CheckIfSkipped()


end

function SetDialogueValues()

   -- Get Dialogue Values From JSON
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

			if(targetID ~= -1) then
				id = targetID
				SetDialogueValues()	
			else
			DeleteGameObjectByUID(Find("Dialogue1"):GetUID())
			end
		end
	end
end

print("Dialogue Script Load Success")