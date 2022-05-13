id = 1
local idIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
idIV = InspectorVariable.new("id", idIVT, id)
NewVariable(idIV)

local src = " "
local char = " "
local line1 = " "
local line2 = " "

path = "Assets/Dialogues/dialogues.json"
prefabPath= "Dialogue"
target = nil

function Start()
	LoadJsonFile(path)

	src = GetDialogueString("src",id)
	char = GetDialogueString("char",id)
	line1 = GetDialogueString("line1", id)
	line2 = GetDialogueString("line2", id)
	InstantiatePrefab(prefabPath)

end

function Update(dt)

	Find("DialogueAvatar"):GetImage():SetTexture(src)
	Find("DialogueName"):GetText():SetTextValue(char)
	Find("DialogueLine1"):GetText():SetTextValue(line1)
	Find("DialogueLine2"):GetText():SetTextValue(line2)

	skipButton = Find("DialogueSkipButton"):GetButton():IsPressed()

	if (skipButton == true) then
	print("clicked")
	DeleteGameObjectByUID(Find("Dialogue1"):GetUID())
	end

end

print("Dialogue Script Load Success")