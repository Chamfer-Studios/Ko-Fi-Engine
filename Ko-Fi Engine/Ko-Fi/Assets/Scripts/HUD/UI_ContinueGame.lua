levelToLoad = 1

function Start()
    LoadGameState()

    levelToLoad = GetGameJsonInt("level_progression")

    if(levelToLoad == 0) then
        DeleteGameObject()
    elseif (levelToLoad == 1) then
        levelToLoad = "SceneLoadingLevel1"
    elseif (levelToLoad == 2) then
        levelToLoad = "SceneLoadingLevel2"
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