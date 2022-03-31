------------------- Variables --------------------

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
			end
		end
end

function PostUpdate(dt)
	aiming = GetVariable("Zhib.lua", "currentAction", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
	if (aiming  ~= 4) then
		
		if (GetInput(1) == KEY_STATE.KEY_DOWN) then
			print("Deselected")
			characterSelected = -1
		end
	end
end

--------------------------------------------------

print("GameState.lua compiled succesfully")