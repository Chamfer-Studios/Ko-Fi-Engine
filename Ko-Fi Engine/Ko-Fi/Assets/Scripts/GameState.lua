------------------- Variables --------------------

characterSelected = 1

-------------------- Methods ---------------------
function Start()
	characters = { Find("Zhib"), Find("Nerala"), Find("Omozra") }
	characterSelectedMesh = Find("CharacterSelectedMesh")
end

-- Called each loop iteration
function Update(dt)

	currentState = GetRuntimeState()
	if (currentState == RuntimeState.PLAYING) then
		if (GetInput(1) == KEY_STATE.KEY_DOWN) then
			local goHovered = GetGameObjectHovered()
			if (goHovered.tag == Tag.PLAYER) then
				if (goHovered:GetName() == "Zhib") then
					characterSelected = 1
				elseif (goHovered:GetName() == "Nerala") then
					characterSelected = 2
				elseif (goHovered:GetName() == "Omozra") then
					characterSelected = 3
				end
			end
		-- Z
		elseif (GetInput(6) == KEY_STATE.KEY_DOWN) then
			if (characterSelected == 1) then
				characterSelected = 0
			else
				characterSelected = 1
			end
		-- X
		elseif (GetInput(8) == KEY_STATE.KEY_DOWN) then
			if (characterSelected == 2) then
				characterSelected = 0
			else
				characterSelected = 2
			end
		-- C
		elseif (GetInput(9) == KEY_STATE.KEY_DOWN) then
			if (characterSelected == 3) then
				characterSelected = 0
			else
				characterSelected = 3
			end
		end
		if (characterSelected ~= 0) then
			playerPos = characters[characterSelected]:GetTransform():GetPosition() 
			characterSelectedMesh:GetTransform():SetPosition(float3.new(playerPos.x, playerPos.y + 30, playerPos.z))
		else 
			characterSelectedMesh:GetTransform():SetPosition(float3.new(characterSelectedMesh:GetTransform():GetPosition().x, -20, characterSelectedMesh:GetTransform():GetPosition().z))
		end
	else
		characterSelected = 0
	end
end

function EventHandler(key, fields)
    if key == "Character_Selected" then -- fields[1] -> characterSelected;
		characterSelected = fields[1]
		--characterSelected = 1
    end
end
--------------------------------------------------

print("GameState.lua compiled succesfully")