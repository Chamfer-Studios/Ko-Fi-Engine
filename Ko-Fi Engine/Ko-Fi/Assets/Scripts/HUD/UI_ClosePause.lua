name = "Pause Menu"

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
            ToggleRuntime()
            child:Active(false)
        end
    end
end

print("UI_ClosePause.lua compiled succesfully")
