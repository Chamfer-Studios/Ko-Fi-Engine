levelToLoad = 1

function Start()
    LoadGameState()

    levelToLoad = GetGameJsonInt("level_progression")

    if(levelToLoad == 1) then
        levelToLoad = "Level1"
    elseif (levelToLoad == 2) then
        levelToLoad = "Level2"
    end
end

-- Called each loop iteration
function Update(dt)
	if (gameObject.active == true) then
		if (gameObject:GetButton():IsPressed() == true) then
			gameObject:ChangeScene(true, levelToLoad)
		end
	end
end

print("UI_ContinueGame.lua compiled successfully!")