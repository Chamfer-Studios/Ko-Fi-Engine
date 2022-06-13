name = "Settings Menu"

local nameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
nameIV = InspectorVariable.new("name", nameIVT, name)
NewVariable(nameIV)

function Start()
    child = Find(name)
    own = Find("Background")
    child:Active(false)
end

-- Called each loop iteration
function UpdateUI(dt)
    if (gameObject.active == true) then
        if (gameObject:GetButton():IsPressed()) then
            child:Active(true)
            own:Active(false)
        end
    end
end

print("UI_OpenSettings.lua compiled succesfully")
