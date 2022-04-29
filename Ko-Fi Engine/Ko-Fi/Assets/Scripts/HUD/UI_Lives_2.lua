lives = 1
path =  "Assets/HUD/slider_segments_none_v1.0.png"

-- Called each loop iteration
function Update(dt)
	if (GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) == 1) then
		currentLives = GetVariable("TemporaryPlayer.lua", "lives", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
		if (currentLives == lives) then
			gameObject:GetImage():SetTexture(path) -- It would be nice if it worked with events instead of every frame
		end
		if (currentLives > lives) then
			gameObject:GetImage():SetTexture("Assets/HUD/slider_segments_hp2_v1.0.png") -- It would be nice if it worked with events instead of every frame
		end
	end
	if (GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) == 2) then
		currentLives = GetVariable("TemporaryPlayer2.lua", "lives", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
		if (currentLives == lives) then
			gameObject:GetImage():SetTexture(path) -- It would be nice if it worked with events instead of every frame
		end
		if (currentLives > lives) then
			gameObject:GetImage():SetTexture("Assets/HUD/slider_segments_hp2_v1.0.png") -- It would be nice if it worked with events instead of every frame
		end
	end
	if (GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) == 3) then
		currentLives = GetVariable("TemporaryPlayer3.lua", "lives", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
		if (currentLives == lives) then
			gameObject:GetImage():SetTexture(path) -- It would be nice if it worked with events instead of every frame
		end
		if (currentLives > lives) then
			gameObject:GetImage():SetTexture("Assets/HUD/slider_segments_hp2_v1.0.png") -- It would be nice if it worked with events instead of every frame
		end
	end
end

print("UI_Lives_2.lua compiled succesfully")