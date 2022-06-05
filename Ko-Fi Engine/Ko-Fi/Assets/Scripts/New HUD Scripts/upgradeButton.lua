name = "purchased"

function Update(dt)
    if (isStarting == true) then
		child = gameObject:GetChild(name)
		child:Active(false)
		isStarting = false
	end
    if (gameObject:GetButton():IsPressed()) then
        child:Active(true)
    end
end
