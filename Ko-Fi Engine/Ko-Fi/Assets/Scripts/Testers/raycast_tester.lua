function Start()
end

function Update()
    RayCast(float3.new(60, -30, 1000), float3.new(60, -30, -1000), "terrain", gameObject, Callback)
    DrawLine(float3.new(60, -30, 1000), float3.new(60, -30, -1000))
end

function Callback()
    Log("hit\n")
end