isStarting = true

-- Called each loop iteration
function Update(dt)
    if (isStarting == true) then
        startTime = os.time()
        endTime = startTime+2
        isStarting = false
    end
    if os.time() >= endTime then
        gameObject:ChangeScene(true, "SceneTutorial_new")
    end
end
    
print("UI_LoadingLvl1.lua compiled succesfully")