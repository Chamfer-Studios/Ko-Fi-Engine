------------------- Variables --------------------

characterSelected = 0

-------------------- Methods ---------------------

-- Called each loop iteration
function Update(dt)

	currentState = GetRuntimeState()
	if (currentState == RuntimeState.PLAYING) then
		if (GetInput(21) == KEY_STATE.KEY_DOWN) then
			if (characterSelected == 1) then
				characterSelected = 0
			else
				characterSelected = 1
			end
		elseif (GetInput(22) == KEY_STATE.KEY_DOWN) then
			if (characterSelected == 2) then
				characterSelected = 0
			else
				characterSelected = 2
			end
		elseif (GetInput(23) == KEY_STATE.KEY_DOWN) then
			if (characterSelected == 3) then
				characterSelected = 0
			else
				characterSelected = 3
			end
		end
	else
		characterSelected = 0
	end
end

--------------------------------------------------

print("GameState.lua compiled succesfully")