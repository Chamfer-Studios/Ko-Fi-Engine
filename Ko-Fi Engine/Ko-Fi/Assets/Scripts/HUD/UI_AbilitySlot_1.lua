-- player = Find("Character")
--characterID = 1
isActive1 = false
isActive2 = false
isActive3 = false
path1 =  "Assets/HUD/throwable_knife_normal_v1.0.png"
path12 =  "Assets/HUD/throwable_knife_active_v1.0.png"
path2 =  "Assets/HUD/sleeping_dart_normal_v1.0.png"
path22 =  "Assets/HUD/sleeping_dart_active_v1.0.png"
path3 =  "Assets/HUD/radar_worm_normal_v1.0.png"
path32 =  "Assets/HUD/radar_worm_active_v1.0.png"

-- Called each loop iteration
function Update(dt)
	--currentCharacter = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
	--if (currentCharacter == 1) then
		--if (isActive1 == false) then
		--	gameObject:GetImage():SetTexture(path1)
		--end
		--if (GetInput(6) == KEY_STATE.KEY_DOWN) then -- 1
		--	startTime1 = os.time()
		--	endTime1 = startTime1+3
		--	gameObject:GetImage():SetTexture(path12)
		--	isActive1 = true
		--end
		--if (os.time() >= endTime1) then
		--	isActive1 = false
		--end
	--end
	--if (currentCharacter == 2) then
	--	if (isActive2 == false) then
	--		gameObject:GetImage():SetTexture(path2)
	--	end
	--	if (GetInput(6) == KEY_STATE.KEY_DOWN) then -- 1
	--		startTime2 = os.time()
	--		endTime2 = startTime2+3
	--		gameObject:GetImage():SetTexture(path22)
	--		isActive2 = true
	--	end
	--	if (os.time() >= endTime2) then
	--		isActive2 = false
	--	end
	--end
	--if (currentCharacter == 3) then
	--	if (isActive3 == false) then
	--		gameObject:GetImage():SetTexture(path3)
	--	end
	--	if (GetInput(6) == KEY_STATE.KEY_DOWN) then -- 1
	--		startTime3 = os.time()
	--		endTime3 = startTime3+3
	--		gameObject:GetImage():SetTexture(path32)
	--		isActive3 = true
	--	end
	--	if (os.time() >= endTime3) then
	--		isActive3 = false
	--	end
	--end
	
end

function EventHandler(key, fields)
    if key == "Player_Ability" then -- fields[1] -> characterID; fields[2] -> ability n; fields[3] -> ability state
        
        if (fields[1] == 1) then -- Zhib
			if (fields[2] == 0) then
				if (fields[3] == 0) then
					if (isActive1 == true) then
						gameObject:GetImage():SetTexture(path1)
						isActive1 = false
					end
				end
			end
			if (fields[2] == 1) then
				if (fields[3] == 1) then
					gameObject:GetImage():SetTexture(path12)
					isActive1 = true
				end
			end
        elseif (fields[1] == 2) then -- Nerala
			if (fields[2] == 0) then
				if (fields[3] == 0) then
					if (isActive2 == true) then
						gameObject:GetImage():SetTexture(path2)
						isActive2 = false
					end
				end
			end
			if (fields[2] == 1) then
				if (fields[3] == 1) then
					gameObject:GetImage():SetTexture(path22)
					isActive2 = true
				end
			end
        elseif (fields[1] == 3) then -- Omozra
			if (fields[2] == 0) then
				if (fields[3] == 0) then
					if (isActive3 == true) then
						gameObject:GetImage():SetTexture(path3)
						isActive3 = false
					end
				end
			end
			if (fields[2] == 1) then
				if (fields[3] == 1) then
					gameObject:GetImage():SetTexture(path32)
					isActive3 = true
				end
			end
        end
    end
end

print("UI_AbilitySlot_1.lua compiled succesfully")