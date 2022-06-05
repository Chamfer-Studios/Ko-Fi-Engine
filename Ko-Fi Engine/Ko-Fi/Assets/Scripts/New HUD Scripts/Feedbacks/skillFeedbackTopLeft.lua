name = "purchased"
isStarting = true

function Update(dt)
    if (isStarting == true) then
		parent = gameObject:GetParent()
		gameObject:Active(false)
        button = Find("Button")
		isStarting = false
	end

    if (button:GetButton():IsIdle() == false and button:GetButton():IsPressed() == false) then
        parent:Active(false)
        gameObject:Active(true)
    else
        parent:Active(true)
        gameObject:Active(false)
    end
end
