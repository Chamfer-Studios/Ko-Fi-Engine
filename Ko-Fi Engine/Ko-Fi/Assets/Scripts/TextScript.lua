player = Find("Character")
hp = GetInt("Player.lua", "hp")

-- Called each loop iteration
function Update(dt)

	gameObject:GetText():SetTextValue(string.format("%s", hp)) -- It would be nice if it worked with events instead of every frame

end

print("TextScript.lua compiled succesfully")