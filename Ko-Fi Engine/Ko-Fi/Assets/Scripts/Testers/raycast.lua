a = float3.new(1000, 0, 800)
b = float3.new(-1000, 0, 800)

function Update(dt)

    lambda = function(uid)
        Log("hello\n")
    end

    RayCastLambda(a, b, "terrain", gameObject, RNG(), lambda)

end