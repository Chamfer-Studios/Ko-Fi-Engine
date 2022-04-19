-- Called each loop iteration
function Update(dt)
	if (gameObject:GetButton():IsPressed() == true) then
		gameObject:OnStoped()
	end
end

print("UI_Quit.lua compiled succesfully")