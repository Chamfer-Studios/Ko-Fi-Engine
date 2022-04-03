-- player = Find("Character")


-- Called each loop iteration
function Update(dt)
	if (gameObject:GetButton():IsPressed() == true) then
		-- gameObject:LoadScene("HUD_Scene")
		gameObject:ChangeScene(true, "Level_1");
	end
end

print("UI_ChangeScenes.lua compiled succesfully")

