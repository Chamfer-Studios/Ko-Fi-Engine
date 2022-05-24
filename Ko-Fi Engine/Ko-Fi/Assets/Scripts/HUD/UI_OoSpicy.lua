CharacterSelected = 1
--spicyMama = 0
--restartSpice = 0

function Start()
	--restartSpice = SpicyMama
end

-- Called each loop iteration
function Update(dt)
	if (gameObject.active == true) then
		spiceAmount = GetVariable("UI_GameState.lua", "spice", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
		gameObject:GetText():SetTextValue(spiceAmount .. " Sp")
		-- if (GetVariable("UI_RestartButton.lua", "restart", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true) then
		-- 	SpicyMama = restartSpice
		-- end
	end
end

print("UI_OoSpicy.lua compiled succesfully")