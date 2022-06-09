transitionTo = "noScene"
transitionToIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
transitionToIV = InspectorVariable.new("transitionTo", transitionToIVT, transitionTo)
NewVariable(transitionToIV)

function Start()

    isTransitioning = false

    transform = gameObject:GetTransform2D()

    transitionSpeed = 666 * 0.001

    transform:SetMask(float2.new(0, 1))
end

function Update(dt)

    if (isTransitioning == true) then
        if (transform:GetMask().x < 1) then
            transform:SetMask(float2.new(transform:GetMask().x + transitionSpeed * dt, 1))
        elseif (transitionTo ~= "noScene") then
            gameObject:ChangeScene(true, transitionTo)
        end
    elseif (GetInput(1) == KEY_STATE.KEY_DOWN or GetInput(43) == KEY_STATE.KEY_DOWN) then
        isTransitioning = true
    end
end

Log("IntroBegin.lua\n")
