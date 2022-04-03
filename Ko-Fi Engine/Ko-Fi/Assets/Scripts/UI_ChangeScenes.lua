-- player = Find("Character")


-- Called each loop iteration
function Update(dt)
	if (gameObject:GetButton():IsPressed() == true) then
		-- gameObject:LoadScene("HUD_Scene")
		gameObject:ChangeScene(true, "HUD_Scene");
	end
end

print("UI_ChangeScenes.lua compiled succesfully")