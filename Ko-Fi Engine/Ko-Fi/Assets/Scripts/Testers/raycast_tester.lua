function Start()
end

function Update()
    hit = CustomRayCast(float3.new(-1000, 0, 800), float3.new(1000, 0, 800), Tag.WALL)

    Log(tostring(hit) .. "\n")
end