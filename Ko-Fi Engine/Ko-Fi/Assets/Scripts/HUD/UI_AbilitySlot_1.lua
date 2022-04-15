-- player = Find("Character")
--characterID = 1
path1 =  "Assets/HUD/throwable_knife_normal_v1.0.png"
path2 =  "Assets/HUD/decoy_normal_v1.0.png"
path3 =  "Assets/HUD/zhib_ultimate_normal_v1.0.png"

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
	--if (currentCharacter ~= 1 and currentCharacter ~= 2 and currentCharacter ~= 3) then
		--gameObject:GetImage():SetTexture
	--end
end

print("UI_AbilitySlot_1.lua compiled succesfully")