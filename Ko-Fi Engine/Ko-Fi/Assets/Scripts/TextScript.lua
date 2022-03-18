player = Find("Character")


-- Called each loop iteration
function Update(dt)
	bullets = GetInt("Player.lua", "bullets")
	gameObject:GetText():SetTextValue(string.format("%s", bullets)) -- It would be nice if it worked with events instead of every frame

end

print("TextScript.lua compiled succesfully")