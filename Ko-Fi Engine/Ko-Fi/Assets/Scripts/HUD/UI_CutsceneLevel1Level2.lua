isActive = 1
change = false
isStarting = true

-- Called each loop iteration
function Update(dt)
    if (isStarting == true) then
        startTime = os.time()
        endTime = startTime+20
        isStarting = false
        change = true
    end
    if os.time() >= endTime then
        isActive = isActive + 1
        isStarting = true
    end
    if (GetInput(44) == KEY_STATE.KEY_DOWN) then -- ENTER
        isActive = 17
        change = true
    end
    if (GetInput(43) == KEY_STATE.KEY_DOWN) then -- SPACE
        isActive = isActive + 1
        isStarting = true
    end
    if (change == true) then
        if (isActive == 1) then
            gameObject:GetChild("Image"):GetImage():SetTexture("Assets/UI/Cutscenes/cutscene_5.png")
            gameObject:GetChild("Text1"):GetChild("Text1Part1"):Active(true)
            gameObject:GetChild("Text2"):Active(false)
            gameObject:GetChild("Text3"):Active(false)
            gameObject:GetChild("Text4"):Active(false)
            gameObject:GetChild("Text5"):Active(false)
            gameObject:GetChild("Text6"):Active(false)
        elseif (isActive == 2) then
            gameObject:GetChild("Image"):GetImage():SetTexture("Assets/UI/Cutscenes/cutscene_6.png")
            gameObject:GetChild("Text1"):Active(false)
            gameObject:GetChild("Text2"):GetChild("Text2Part1"):Active(true)
        elseif (isActive == 3) then
            gameObject:GetChild("Image"):GetImage():SetTexture("Assets/UI/Cutscenes/cutscene_8.png")
            gameObject:GetChild("Text2"):Active(false)
            gameObject:GetChild("Text4"):GetChild("Text4Part1"):Active(true)
        elseif (isActive == 4) then
            gameObject:GetChild("Image"):GetImage():SetTexture("Assets/UI/Cutscenes/cutscene_9.png")
            gameObject:GetChild("Text4"):Active(false)
            gameObject:GetChild("Text5"):GetChild("Text5Part1"):Active(true)
        elseif (isActive == 5) then
            gameObject:GetChild("Text5"):GetChild("Text5Part1"):Active(false)
            gameObject:GetChild("Text5"):GetChild("Text5Part2"):Active(true)
        elseif (isActive == 6) then
            gameObject:GetChild("Image"):GetImage():SetTexture("Assets/UI/Cutscenes/cutscene_10.png")
            gameObject:GetChild("Text5"):Active(false)
            gameObject:GetChild("Text6"):GetChild("Text6Part1"):Active(true)
        elseif (isActive == 7) then
            gameObject:GetChild("Text6"):GetChild("Text6Part1"):Active(false)
            gameObject:GetChild("Text6"):GetChild("Text6Part2"):Active(true)
        elseif (isActive >= 8) then
            gameObject:ChangeScene(true, "LevelArrakeen")
        end
        change = false
    end
end
    
print("UI_CutsceneLevel1Level2.lua compiled succesfully")