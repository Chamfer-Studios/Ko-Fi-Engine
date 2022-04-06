-- player = Find("Character")
lives = 0
path =  "Assets/HUD/slider_segments_hp3_v1.0.png"

local livesIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
livesIV = InspectorVariable.new("lives", livesIVT, lives)
NewVariable(livesIV)

local pathIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
pathIV = InspectorVariable.new("path", pathIVT, path)
NewVariable(pathIV)

-- Called each loop iteration
function Update(dt)
	if (GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) == 1) then
	print("zhib")
		currentLives = GetVariable("TemporaryPlayer.lua", "lives", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
		if (currentLives == lives) then
			gameObject:GetImage():SetTexture(path) -- It would be nice if it worked with events instead of every frame
		end
	end
	if (GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) == 2) then
		currentLives = GetVariable("TemporaryPlayer1.lua", "lives", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
		if (currentLives == lives) then
			gameObject:GetImage():SetTexture(path) -- It would be nice if it worked with events instead of every frame
		end
	end
	if (GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) == 3) then
		currentLives = GetVariable("TemporaryPlayer2.lua", "lives", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
		if (currentLives == lives) then
			gameObject:GetImage():SetTexture(path) -- It would be nice if it worked with events instead of every frame
		end
	end
end

print("UI_Lives.lua compiled succesfully")