zhibHP = 0
zhibMaxHP = 0
neralaHP = 0
neralaMaxHP = 0
omozraHP = 0
omozraMaxHP = 0
pathRedHP =  "Assets/UI/HUD/Slider Segments/slider_segments_hp1_v1.0.png"
pathGreyHP = "Assets/UI/HUD/Slider Segments/slider_segments_hp3_v1.0.png"
pathRed =  "Assets/UI/HUD/Slider Segments/slider_segments_hp2_v1.0.png"
pathGrey = "Assets/UI/HUD/Slider Segments/slider_segments_none_v1.0.png"
pathBlank = "Assets/UI/HUD/blank_asset_v1.0.png"

-- Called each loop iteration
function Update(dt)
	if (GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) == 1) then
		if (zhibHP == 3) then
			gameObject:GetImage():SetTexture(pathRedHP)
			gameObject:GetChild("HealthBar_1"):GetImage():SetTexture(pathRed)
			gameObject:GetChild("HealthBar_1"):GetChild("HealthBar_2"):GetImage():SetTexture(pathRed)
		elseif (zhibHP == 2) then
			gameObject:GetImage():SetTexture(pathRedHP)
			gameObject:GetChild("HealthBar_1"):GetImage():SetTexture(pathRed)
			gameObject:GetChild("HealthBar_1"):GetChild("HealthBar_2"):GetImage():SetTexture(pathGrey)
		elseif (zhibHP == 1) then
			gameObject:GetImage():SetTexture(pathRedHP)
			gameObject:GetChild("HealthBar_1"):GetImage():SetTexture(pathGrey)
			gameObject:GetChild("HealthBar_1"):GetChild("HealthBar_2"):GetImage():SetTexture(pathGrey)
		elseif (zhibHP == 0) then
			gameObject:GetImage():SetTexture(pathGreyHP)
			gameObject:GetChild("HealthBar_1"):GetImage():SetTexture(pathGrey)
			gameObject:GetChild("HealthBar_1"):GetChild("HealthBar_2"):GetImage():SetTexture(pathGrey)
		end
	elseif (GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) == 2) then
		if (neralaHP == 3) then
			gameObject:GetImage():SetTexture(pathRedHP)
			gameObject:GetChild("HealthBar_1"):GetImage():SetTexture(pathRed)
			gameObject:GetChild("HealthBar_1"):GetChild("HealthBar_2"):GetImage():SetTexture(pathRed)
		elseif (neralaHP == 2) then
			gameObject:GetImage():SetTexture(pathRedHP)
			gameObject:GetChild("HealthBar_1"):GetImage():SetTexture(pathRed)
			gameObject:GetChild("HealthBar_1"):GetChild("HealthBar_2"):GetImage():SetTexture(pathGrey)
		elseif (neralaHP == 1) then
			gameObject:GetImage():SetTexture(pathRedHP)
			gameObject:GetChild("HealthBar_1"):GetImage():SetTexture(pathGrey)
			gameObject:GetChild("HealthBar_1"):GetChild("HealthBar_2"):GetImage():SetTexture(pathGrey)
		elseif (neralaHP == 0) then
			gameObject:GetImage():SetTexture(pathGreyHP)
			gameObject:GetChild("HealthBar_1"):GetImage():SetTexture(pathGrey)
			gameObject:GetChild("HealthBar_1"):GetChild("HealthBar_2"):GetImage():SetTexture(pathGrey)
		end
	elseif (GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) == 3) then
		if (omozraHP == 3) then
			gameObject:GetImage():SetTexture(pathRedHP)
			gameObject:GetChild("HealthBar_1"):GetImage():SetTexture(pathRed)
			gameObject:GetChild("HealthBar_1"):GetChild("HealthBar_2"):GetImage():SetTexture(pathRed)
		elseif (omozraHP == 2) then
			gameObject:GetImage():SetTexture(pathRedHP)
			gameObject:GetChild("HealthBar_1"):GetImage():SetTexture(pathRed)
			gameObject:GetChild("HealthBar_1"):GetChild("HealthBar_2"):GetImage():SetTexture(pathGrey)
		elseif (omozraHP == 1) then
			gameObject:GetImage():SetTexture(pathRedHP)
			gameObject:GetChild("HealthBar_1"):GetImage():SetTexture(pathGrey)
			gameObject:GetChild("HealthBar_1"):GetChild("HealthBar_2"):GetImage():SetTexture(pathGrey)
		elseif (omozraHP == 0) then
			gameObject:GetImage():SetTexture(pathGreyHP)
			gameObject:GetChild("HealthBar_1"):GetImage():SetTexture(pathGrey)
			gameObject:GetChild("HealthBar_1"):GetChild("HealthBar_2"):GetImage():SetTexture(pathGrey)
		end
	elseif (GetVariable("GameState.lua", "characterSelected", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT) == 0) then
		gameObject:GetImage():SetTexture(pathBlank)
		gameObject:GetChild("HealthBar_1"):GetImage():SetTexture(pathBlank)
		gameObject:GetChild("HealthBar_1"):GetChild("HealthBar_2"):GetImage():SetTexture(pathBlank)
		
	end
end

function EventHandler(key, fields)
    if key == "Player_Health" then -- fields[1] -> characterID; fields[2] -> currentHP; fields[3] -> maxHP
        if (fields[1] == 1) then -- Zhib
			zhibHP = fields[2]
			zhibMaxHP = fields[3]
        elseif (fields[1] == 2) then -- Nerala
			neralaHP = fields[2]
			neralaMaxHP = fields[3]
        elseif (fields[1] == 3) then -- Omozra
			omozraHP = fields[2]
			omozraMaxHP = fields[3]
        end
    end
end

print("UI_Lives.lua compiled succesfully")