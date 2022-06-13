transitionTo = "noScene"
transitionToIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
transitionToIV = InspectorVariable.new("transitionTo", transitionToIVT, transitionTo)
NewVariable(transitionToIV)

function Start()
    timer = 0.0
end
once = false
function Update(dt)
    timer = timer + dt
    if timer >= 4 and once == false then
        if (GetInput(1) == KEY_STATE.KEY_DOWN or GetInput(43) == KEY_STATE.KEY_DOWN) then
            once = true
            gameObject:ChangeScene(true, transitionTo)
        end
    end
end

Log("IntroBegin.lua\n")
