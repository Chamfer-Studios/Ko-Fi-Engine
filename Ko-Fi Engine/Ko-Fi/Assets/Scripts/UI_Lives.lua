-- player = Find("Character")


-- Called each loop iteration
function Update(dt)
	lives = GetVariable("Player.lua", "lives", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
	if (lives == 2) then
		gameObject:GetImage():SetTexture("Assets/Textures/Blue.png") -- It would be nice if it worked with events instead of every frame
	end
end

print("UI_Lives.lua compiled succesfully")