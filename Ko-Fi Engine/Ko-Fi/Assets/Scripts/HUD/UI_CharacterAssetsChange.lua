-- player = Find("Character")
--characterID = 1
path1 =  "Assets/HUD/hud_portrait_main_v1.0.png"
path2 =  "Assets/HUD/hud_portrait_nerala_v1.1.png"
path3 =  "Assets/HUD/hud_portrait_omozra_v1.1.png"

--local characterIDIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
--characterIDIV = InspectorVariable.new("lives", characterIDIVT, characterID)
--NewVariable(characterIDIV)

local path1IVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
path1IV = InspectorVariable.new("path1", path1IVT, path1)
NewVariable(path1IV)

local path2IVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
path2IV = InspectorVariable.new("path2", path2IVT, path2)
NewVariable(path2IV)

local path3IVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
path3IV = InspectorVariable.new("path3", path3IVT, path3)
NewVariable(path3IV)

-- Called each loop iteration
function Update(dt)
	currentCharacter = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
	if (currentCharacter == 1) then
		gameObject:GetImage():SetTexture(path1) -- It would be nice if it worked with events instead of every frame
	end
	if (currentCharacter == 2) then
		gameObject:GetImage():SetTexture(path2) -- It would be nice if it worked with events instead of every frame
	end
	if (currentCharacter == 3) then
		gameObject:GetImage():SetTexture(path3) -- It would be nice if it worked with events instead of every frame
	end
end

print("UI_Lives.lua compiled succesfully")