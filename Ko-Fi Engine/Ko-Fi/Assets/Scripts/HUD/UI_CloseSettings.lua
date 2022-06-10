name = "Settings Menu"

local nameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
nameIV = InspectorVariable.new("name", nameIVT, name)
NewVariable(nameIV)

function Start()
    child = Find(name)
end

-- Called each loop iteration
function UpdateUI(dt)
    if (gameObject.active == true) then
        if (gameObject:GetButton():IsPressed()) then
            child:Active(false)
        end
    end
end

print("UI_CloseSettings.lua compiled succesfully")
