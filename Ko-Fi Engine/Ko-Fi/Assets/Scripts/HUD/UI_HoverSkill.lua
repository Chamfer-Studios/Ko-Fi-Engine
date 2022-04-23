name = "QuestsMenu"
isStarting = true

local nameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
nameIV = InspectorVariable.new("name", nameIVT, name)
NewVariable(nameIV)

-- Called each loop iteration
function Update(dt)
	if (isStarting == true) then
		child = gameObject:GetChild(name)
		child:Active(false)
		isStarting = false
	end
	if (gameObject:GetButton():IsHovered()) then
		child:Active(true)
		--print("hovered")
	end
	if (gameObject:GetButton():IsIdle()) then
		child:Active(false)
		--print("not hovered")
	end
end

print("UI_HoverSkill.lua compiled succesfully")