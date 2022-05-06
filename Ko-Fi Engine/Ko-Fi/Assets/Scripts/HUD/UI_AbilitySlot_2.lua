isActive1 = false
isActive2 = false
isActive3 = false
zhib = 1
nerala = 1
omozra = 1
path1 =  "Assets/HUD/decoy_normal_v1.0.png"
path12 =  "Assets/HUD/decoy_active_v1.0.png"
path13 =  "Assets/HUD/decoy_cooldown_v1.0.png"
path2 =  "Assets/HUD/smoke_bomb_normal_v1.0.png"
path22 =  "Assets/HUD/smoke_bomb_active_v1.0.png"
path23 =  "Assets/HUD/smoke_bomb_cooldown_v1.0.png"
path3 =  "Assets/HUD/hunter_worm_normal_v1.0.png"
path32 =  "Assets/HUD/hunter_worm_active_v1.0.png"
path33 =  "Assets/HUD/hunter_worm_cooldown_v1.0.png"
pathBlank = "Assets/HUD/blank_asset_v1.0.png"

-- Called each loop iteration
function Update(dt)
	if (GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) == 1) then
		if (zhib == 1) then
			gameObject:GetImage():SetTexture(path1)
		elseif (zhib == 2) then
			gameObject:GetImage():SetTexture(path12)
		elseif (zhib == 3) then
			gameObject:GetImage():SetTexture(path13)
		end
	elseif (GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) == 2) then
		if (nerala == 1) then
			gameObject:GetImage():SetTexture(path2)
		elseif (nerala == 2) then
			gameObject:GetImage():SetTexture(path22)
		elseif (nerala == 3) then
			gameObject:GetImage():SetTexture(path23)
		end
	elseif (GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) == 3) then
		if (omozra == 1) then
			gameObject:GetImage():SetTexture(path3)
		elseif (omozra == 2) then
			gameObject:GetImage():SetTexture(path32)
		elseif (omozra == 3) then
			gameObject:GetImage():SetTexture(path33)
		end
	elseif (GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) == 0) then
		gameObject:GetImage():SetTexture(pathBlank)
	end
end

function EventHandler(key, fields)
    if key == "Player_Ability" then -- fields[1] -> characterID; fields[2] -> ability n; fields[3] -> ability state
        if (fields[1] == 1) then -- Zhib
			if (fields[2] == 0) then
				if (fields[3] == 0) then
					if (isActive1 == true) then
						zhib = 1
						isActive1 = false
					end
				end
			end
			if (fields[2] == 2) then
				if (fields[3] == 1) then
					zhib = 2
					isActive1 = true
				elseif (fields[3] == 2) then
					zhib = 3
					isActive1 = false
				end
			end
        elseif (fields[1] == 2) then -- Nerala
			if (fields[2] == 0) then
				if (fields[3] == 0) then
					if (isActive2 == true) then
						nerala = 1
						isActive2 = false
					end
				end
			end
			if (fields[2] == 2) then
				if (fields[3] == 1) then
					nerala = 2
					isActive2 = true
				elseif (fields[3] == 2) then
					nerala = 3
					isActive2 = false
				end
			end
        elseif (fields[1] == 3) then -- Omozra
			if (fields[2] == 0) then
				if (fields[3] == 0) then
					if (isActive3 == true) then
						omozra = 1
						isActive3 = false
					end
				end
			end
			if (fields[2] == 2) then
				if (fields[3] == 1) then
					omozra = 2
					isActive3 = true
				elseif (fields[3] == 2) then
					omozra = 3
					isActive3 = false
				end
			end
        end
    end
end

print("UI_AbilitySlot_2.lua compiled succesfully")