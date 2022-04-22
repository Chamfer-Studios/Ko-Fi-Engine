name = "SkillsMenu"
isStarting = true

-- Called each loop iteration
function Update(dt)
	if (isStarting == true) then
		child = gameObject:GetParent():GetChild(name)
		child:Active(false)
		isStarting = false
	end
	if (gameObject:GetParent():GetChild("SkillsMenu").active == false) then
		if (gameObject:GetButton():IsPressed()) then
			child:Active(true)
		end
	end
end

print("UI_OpenPopUp.lua compiled succesfully")