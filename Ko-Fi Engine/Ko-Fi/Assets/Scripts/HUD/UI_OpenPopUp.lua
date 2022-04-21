name = "SkillsMenu"
estoesnboom = true

-- Called each loop iteration
function Update(dt)
	if (estoesnboom == true) then
		child = gameObject:GetParent():GetChild(name)
		child:Active(false)
		print(child:GetName())
		estoesnboom = false
	end
	if (gameObject:GetButton():IsPressed()) then
		if (child.active == false) then
			child:Active(true)
		end
		if (child.active == true) then
			child:Active(false)
		end
	end
end

print("UI_OpenPopUp.lua compiled succesfully")