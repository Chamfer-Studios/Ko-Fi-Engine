name = "Settings Menu"

local nameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
nameIV = InspectorVariable.new("name", nameIVT, name)
NewVariable(nameIV)

function Start()
    child = Find(name)
    child:Active(false)
end

-- Called each loop iteration
function UpdateUI(dt)
    if (gameObject.active == true) then
        if (gameObject:GetButton():IsPressed()) then
            child:Active(true)
        end
    end
end

print("UI_OpenSettings.lua compiled succesfully")
