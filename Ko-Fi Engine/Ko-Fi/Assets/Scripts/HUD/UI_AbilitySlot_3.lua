-- player = Find("Character")
--characterID = 1
isActive1 = false
isActive2 = false
isActive3 = false
path1 =  "Assets/HUD/zhib_ultimate_normal_v1.0.png"
path12 =  "Assets/HUD/zhib_ultimate_active_v1.0.png"
path2 =  "Assets/HUD/nerala_ultimate_normal_v1.0.png"
path22 =  "Assets/HUD/nerala_ultimate_active_v1.0.png"
path3 =  "Assets/HUD/omozra_ultimate_normal_v1.0.png"
path32 =  "Assets/HUD/omozra_ultimate_active_v1.0.png"

-- Called each loop iteration
function Update(dt)
	currentCharacter = GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
	if (currentCharacter == 1) then
		if (isActive1 == false) then
			gameObject:GetImage():SetTexture(path1)
		end
		if (GetInput(4) == KEY_STATE.KEY_DOWN) then -- 1
			startTime1 = os.time()
			endTime1 = startTime1+3
			gameObject:GetImage():SetTexture(path12)
			isActive1 = true
		end
		if (os.time() >= endTime1) then
			isActive1 = false
		end
	end
	if (currentCharacter == 2) then
		if (isActive2 == false) then
			gameObject:GetImage():SetTexture(path2)
		end
		if (GetInput(4) == KEY_STATE.KEY_DOWN) then -- 1
			startTime2 = os.time()
			endTime2 = startTime2+3
			gameObject:GetImage():SetTexture(path22)
			isActive2 = true
		end
		if (os.time() >= endTime2) then
			isActive2 = false
		end
	end
	if (currentCharacter == 3) then
		if (isActive3 == false) then
			gameObject:GetImage():SetTexture(path3)
		end
		if (GetInput(4) == KEY_STATE.KEY_DOWN) then -- 1
			startTime3 = os.time()
			endTime3 = startTime3+3
			gameObject:GetImage():SetTexture(path32)
			isActive3 = true
		end
		if (os.time() >= endTime3) then
			isActive3 = false
		end
	end
	
end

print("UI_AbilitySlot_1.lua compiled succesfully")