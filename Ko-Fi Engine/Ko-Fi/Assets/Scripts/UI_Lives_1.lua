-- player = Find("Character")
lives = 0

-- local livesIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
-- local livesIV = InspectorVariable.new("lives", livesIVT, lives)
-- NewVariable(livesIV)

-- Called each loop iteration
function Update(dt)
	currentLives = GetVariable("Player.lua", "lives", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
	if (currentLives == lives) then
		gameObject:GetImage():SetTexture("Assets/HUD/slider_segments_hp3_v1.0.png") -- It would be nice if it worked with events instead of every frame
	end
end

print("UI_Lives_1.lua compiled succesfully")