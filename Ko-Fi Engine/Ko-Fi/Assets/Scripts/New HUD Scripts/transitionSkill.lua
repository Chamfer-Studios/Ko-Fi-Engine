function Update(dt)
    if (gameObject:GetButton():IsPressed()) then
        --if(gameObject:GetName() == "Button") then
            --DispatchGlobalEvent("FirstHovered")
        --elseif (gameObject:GetName() == "Button(1)") then
            --DispatchGlobalEvent("SecondHovered")
        --elseif (gameObject:GetName() == "Button(2)") then
            --DispatchGlobalEvent("ThirdHovered")
        --end
        DispatchGlobalEvent("FirstHovered")

    end
end
