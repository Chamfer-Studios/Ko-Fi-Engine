isStarting = true

-- Called each loop iteration
function Update(dt)
    if (isStarting == true) then
        startTime = os.time()
        endTime = startTime+2
        isStarting = false
    end
    if os.time() >= endTime then
        gameObject:ChangeScene(true, "SceneArrakeen")
    end
end
    
print("UI_LoadingLvl2.lua compiled succesfully")