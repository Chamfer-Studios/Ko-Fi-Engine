player = Find("Character")


-- Called each loop iteration
function Update(dt)
	bullets = GetVariable("Player.lua", "bullets", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
	gameObject:GetText():SetTextValue(string.format("%s", bullets)) -- It would be nice if it worked with events instead of every frame

end

function PostUpdate(dt)

end

print("TextScript.lua compiled succesfully")