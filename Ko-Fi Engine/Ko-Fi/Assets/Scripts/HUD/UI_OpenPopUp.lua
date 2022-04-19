name = "SkillsMenu"
child = gameObject:GetParent():GetChild(name)
child.active = false

-- Called each loop iteration
function Update(dt)
	if (gameObject:GetButton():IsPressed()) then -- SPACE
		child.active = true
	end
end

print("UI_OpenPopUp.lua compiled succesfully")