transitionTo = "noScene"
transitionToIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
transitionToIV = InspectorVariable.new("transitionTo", transitionToIVT, transitionTo)
NewVariable(transitionToIV)

function Update(dt)

    if (GetInput(1) == KEY_STATE.KEY_DOWN or GetInput(43) == KEY_STATE.KEY_DOWN) then
        gameObject:ChangeScene(true, transitionTo)
    end

end

Log("IntroBegin.lua\n")
