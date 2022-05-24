--starts = false
price = 200
skillName = "ability"
lvlNumber = 0
purchased = false
name = "Shady"
isStarting = true

local nameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
nameIV = InspectorVariable.new("name", nameIVT, name)
NewVariable(nameIV)

-- local startsIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL
-- startsIV = InspectorVariable.new("starts", startsIVT, starts)
-- NewVariable(startsIV)

local priceIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
priceIV = InspectorVariable.new("price", priceIVT, price)
NewVariable(priceIV)

local skillNameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
skillNameIV = InspectorVariable.new("skillName", skillNameIVT, skillName)
NewVariable(skillNameIV)

local lvlNumberIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
lvlNumberIV = InspectorVariable.new("lvlNumber", lvlNumberIVT, lvlNumber)
NewVariable(lvlNumberIV)

-- Called each loop iteration
function Update(dt)
	-- if (GetVariable("UI_RestartButton.lua", "restart", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true) then
	-- 	isStarting = true
	-- end
	if (GetVariable("UI_GameState.lua", skillName, INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) >= lvlNumber) then
		purchased = true
	end
	if (gameObject.active == true) then
		-- if (isStarting == true) then
		-- 	purchased = starts
		-- 	isStarting = false
		-- end
		if (gameObject:GetButton():IsPressed() == true) then
			skillLvl = GetVariable("UI_GameState.lua", skillName, INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
			if ((lvlNumber-1) == skillLvl and purchased == false and GetVariable("UI_GameState.lua", "spice", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) >= price) then
				OGSpice = GetVariable("UI_GameState.lua", "spice", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
				NewSpice = OGSpice - price
				
				SetVariable(NewSpice, "UI_GameState.lua", "spice", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
				SetVariable(lvlNumber, "UI_GameState.lua", skillName, INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
				
				DispatchGlobalEvent("Save_Game", {})
				
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