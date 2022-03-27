-- player = Find("Character")


-- Called each loop iteration
function Update(dt)
	lives = GetInt("Player.lua", "lives")
	print(lives)
	if (lives == 2) then
		gameObject:GetImage():SetTexture("Assets/Textures/Blue.png") -- It would be nice if it worked with events instead of every frame
	end
end

print("UI_Lives.lua compiled succesfully")