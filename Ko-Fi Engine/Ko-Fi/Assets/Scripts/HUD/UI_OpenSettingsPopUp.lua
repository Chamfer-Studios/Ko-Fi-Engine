name = "Settings Menu"
popUp = false

local nameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
nameIV = InspectorVariable.new("name", nameIVT, name)
NewVariable(nameIV)

function Start()
    child = Find(name)
    child:Active(false)
end

-- Called each loop iteration
function UpdateUI(dt)
    if (gameObject:GetButton():IsPressed()) then
        popUp = not popUp
        child:Active(popUp)
    end
end

print("UI_OpenSettingsPopUp.lua compiled succesfully")
