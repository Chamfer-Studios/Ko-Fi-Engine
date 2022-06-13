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
        isActive = 14
        change = true
    end
    if (GetInput(43) == KEY_STATE.KEY_DOWN) then -- SPACE
        isActive = isActive + 1
        isStarting = true
    end
    if (change == true) then
        if (isActive == 1) then
            gameObject:GetChild("Image"):GetImage():SetTexture("Assets/UI/Cutscenes/cutscene_11.png")
            gameObject:GetChild("Text1"):GetChild("Text1Part1"):Active(true)
            gameObject:GetChild("Text1"):GetChild("Text1Part2"):Active(false)
            gameObject:GetChild("Text2"):Active(false)
            gameObject:GetChild("Text3"):Active(false)
            gameObject:GetChild("Text4"):Active(false)
        elseif (isActive == 2) then
            gameObject:GetChild("Image"):GetImage():SetTexture("Assets/UI/Cutscenes/cutscene_12.png")
            gameObject:GetChild("Text1"):Active(false)
            gameObject:GetChild("Text2"):GetChild("Text2Part1"):Active(true)
        elseif (isActive == 3) then
            gameObject:GetChild("Image"):GetImage():SetTexture("Assets/UI/Cutscenes/cutscene_13.png")
            gameObject:GetChild("Text2"):Active(false)
            gameObject:GetChild("Text3"):GetChild("Text3Part1"):Active(true)
        elseif (isActive == 4) then
            gameObject:GetChild("Text3"):GetChild("Text3Part1"):Active(false)
            gameObject:GetChild("Text3"):GetChild("Text3Part2"):Active(true)
        elseif (isActive == 5) then
            gameObject:GetChild("Image"):GetImage():SetTexture("Assets/UI/Cutscenes/cutscene_14.png")
            gameObject:GetChild("Text3"):Active(false)
            gameObject:GetChild("Text4"):GetChild("Text4Part1"):Active(true)
        elseif (isActive == 6) then
            gameObject:GetChild("Text4"):GetChild("Text4Part1"):Active(false)
            gameObject:GetChild("Text4"):GetChild("Text4Part2"):Active(true)
        elseif (isActive == 7) then
            gameObject:GetChild("Text4"):GetChild("Text4Part2"):Active(false)
            gameObject:GetChild("Text4"):GetChild("Text4Part3"):Active(true)
        elseif (isActive == 8) then
            gameObject:GetChild("Text4"):GetChild("Text4Part3"):Active(false)
            gameObject:GetChild("Text4"):GetChild("Text4Part4"):Active(true)
        elseif (isActive == 9) then
            gameObject:ChangeScene(true, "Scene")
        end
        change = false
    end
end
    
print("UI_CutsceneNeralasDeath.lua compiled succesfully")