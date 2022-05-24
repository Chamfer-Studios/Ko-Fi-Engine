spiceAmount = 0
spiceMaxLvl1 = 10000
function Start()
    spiceAmount = GetVariable("GameState.lua", "spiceAmount", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
    spiceMaxLvl1 = GetVariable("GameState.lua", "spiceMaxLvl1", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
end


function Update(dt)
    
    spiceAmount = GetVariable("GameState.lua", "spiceAmount", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
    maskValue = spiceAmount/spiceMaxLvl1
    gameObject:GetTransform2D():SetMask(float2.new(maskValue,1))
end