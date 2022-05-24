name = "Level_1"

local nameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
nameIV = InspectorVariable.new("name", nameIVT, name)
NewVariable(nameIV)

function Update(dt)
    if GetInput(43) ==  KEY_STATE.KEY_DOWN then
        gameObject:ChangeScene(true, name);
    end
end