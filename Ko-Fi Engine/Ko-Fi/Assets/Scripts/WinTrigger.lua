sceneName = "default_level"

local sceneNameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
sceneNameIV = InspectorVariable.new("sceneName", sceneNameIVT, sceneName)
NewVariable(sceneNameIV)

triggered = false

------------------ Collisions --------------------
function OnTriggerEnter(go)
    if triggered == false then
        local currentLevel = GetVariable("GameState.lua", "levelNumber", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
        Log("In level: " .. currentLevel .. "\n")
        if currentLevel == 1 then
            local neralaAvailable = GetVariable("GameState.lua", "neralaAvailable",
                INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL)
            if neralaAvailable == true then
                Log("Nerala unlocked: True\n")
            else
                Log("Nerala unlocked: False\n")
            end
            local omozraAvailable = GetVariable("GameState.lua", "omozraAvailable",
                INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL)
            if omozraAvailable == true then
                Log("Omozra unlocked: True\n")
            else
                Log("Omozra unlocked: False\n")
            end
            if neralaAvailable == true and omozraAvailable == true then
                if (go.tag == Tag.PLAYER) then
                    Log("Changing scene!\n")
                    triggered = true
                    gameObject:ChangeScene(true, sceneName);
                end
            end
        end
    end
end
--------------------------------------------------

print("WinTrigger.lua compiled succesfully")
