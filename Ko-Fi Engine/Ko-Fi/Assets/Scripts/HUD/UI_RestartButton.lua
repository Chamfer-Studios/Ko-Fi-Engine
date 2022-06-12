restart = false

-- Called each loop iteration
function UpdateUI(dt)
	if (gameObject.active == true) then
		if (restart == true) then
			restart = false
		end
		if (gameObject:GetButton():IsPressed() == true) then
			if (restart == false) then;
				restart = true
			end
		end
	end
end

print("UI_Skill.lua compiled succesfully")