isStarting = true

-- Called each loop iteration
function Update(dt)
	if (isStarting == true) then
		child = gameObject:GetParent()
		isStarting = false
	end
	if (gameObject:GetButton():IsPressed()) then
		child:Active(false)
	end
end

print("UI_ClosePopUp.lua compiled succesfully")