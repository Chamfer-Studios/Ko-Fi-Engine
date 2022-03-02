print("GameStates.lua loaded")

function Update(dt)
	print("Update2")

	rot = gameObject:GetParent():GetTransform():GetRotation()
	rot.x = rot.x + 1
	gameObject:GetParent():GetTransform():SetRotation(rot)

end

print("GameState.lua compiled")