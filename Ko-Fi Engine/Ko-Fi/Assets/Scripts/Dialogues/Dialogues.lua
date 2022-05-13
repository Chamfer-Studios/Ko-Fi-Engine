id = 1
local idIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
idIV = InspectorVariable.new("id", idIVT, id)
NewVariable(idIV)

local src = " "
local char = " "
local lines = {}


function Start()
	LoadJsonFile(path)

	src = GetDialogueString("src",id)
	char = GetDialogueString("char",id)

end

function Update(dt)
	print(src)
	print(char)
end

print("Dialogue Script Load Success")