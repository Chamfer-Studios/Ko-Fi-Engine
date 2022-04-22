-- Called each loop iteration
function Update(dt)
	if (gameObject:GetParent():GetChild("SkillsMenu").active == false) then
		if (gameObject:GetButton():IsPressed() == true) then
			gameObject:OnStoped()
		end
	end
end

print("UI_Quit.lua compiled succesfully")