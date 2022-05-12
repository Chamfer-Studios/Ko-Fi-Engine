id = 0
local idIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
idIV = InspectorVariable.new("id", idIVT, id)
NewVariable(idIV)

path = "Assets/Dialogues/dialogues.json"

function Start()
	LoadJsonFile(path)
	a = GetIntFromJson(path, "id")

end

function Update(dt)

	print(a)
end