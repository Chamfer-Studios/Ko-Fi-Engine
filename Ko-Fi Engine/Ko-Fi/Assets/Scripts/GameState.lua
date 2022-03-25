------------------- Variables --------------------

currentState = GetRuntimeState()
characterSelected = -1

-------------------- Methods ---------------------

-- Called each loop iteration
function Update(dt)
	
	currentState = GetRuntimeState()
	if (currentState == RuntimeState.PLAYING) 
		then
			if (GetInput(21) == KEY_STATE.KEY_DOWN) then
				characterSelected = 1
			elseif (GetInput(22) == KEY_STATE.KEY_DOWN) then
				characterSelected = 2
			elseif (GetInput(23) == KEY_STATE.KEY_DOWN) then
				characterSelected = 3
			elseif (GetInput(24) == KEY_STATE.KEY_DOWN) then
				characterSelected = 4
			elseif (GetInput(1) == KEY_STATE.KEY_DOWN) then
				characterSelected = -1
			end
		end
end

--------------------------------------------------

print("GameState.lua compiled succesfully")