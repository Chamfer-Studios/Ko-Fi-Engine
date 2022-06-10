-- Called each loop iteration
function Update(dt)
	if (gameObject.active == true) then
		if (gameObject:GetButton():IsPressed() == true) then
			gameObject:Quit()
		end
	end
end

function UpdateUI(dt)
	if (gameObject.active == true) then
		if (gameObject:GetButton():IsPressed() == true) then
			gameObject:Quit()
		end
	end
end

print("UI_Quit.lua compiled succesfully")