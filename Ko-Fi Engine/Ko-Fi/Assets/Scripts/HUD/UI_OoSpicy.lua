SpicyMama = 300
restartSpice = 300
CharacterSelected = 1
isStarting = true

-- Called each loop iteration
function Update(dt)
	if (gameObject.active == true) then
		if (isStarting == true) then
			restartSpice = SpicyMama
			isStarting = false
		end
		gameObject:GetText():SetTextValue(SpicyMama .. "Sp")
		if (GetVariable("UI_RestartButton.lua", "restart", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true) then
			SpicyMama = restartSpice
		end
	end
end

print("UI_OoSpicy.lua compiled succesfully")