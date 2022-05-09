starts = false
price = 10
purchased = false
name = "Shady"
isStarting = true

local nameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
nameIV = InspectorVariable.new("name", nameIVT, name)
NewVariable(nameIV)

local startsIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL
startsIV = InspectorVariable.new("starts", startsIVT, starts)
NewVariable(startsIV)

local priceIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
priceIV = InspectorVariable.new("price", priceIVT, price)
NewVariable(priceIV)

-- Called each loop iteration
function Update(dt)
	if (GetVariable("UI_RestartButton.lua", "restart", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true) then
		isStarting = true
	end
	if (gameObject.active == true) then
		if (isStarting == true) then
			purchased = starts
			isStarting = false
		end
		if (gameObject:GetButton():IsPressed() == true) then
			if (purchased == false) then
				OGSpice = GetVariable("UI_OoSpicy.lua", "SpicyMama", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
				NewSpice = OGSpice - price
				SetVariable(NewSpice, "UI_OoSpicy.lua", "SpicyMama", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT);
				purchased = true
			end
		end
		if (purchased == true) then
			--DeleteGameObject()
			--gameObject:GetChild("Text15"):GetText():SetTextValue("false")
			gameObject:GetChild(name).active = false
		end
		if (purchased == false) then
			gameObject:GetChild(name).active = true
		end
	end
end

print("UI_Skill.lua compiled succesfully")