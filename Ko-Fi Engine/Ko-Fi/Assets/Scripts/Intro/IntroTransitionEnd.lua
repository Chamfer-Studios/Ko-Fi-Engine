function Start()

    isTransitioning = true

    transform = gameObject:GetTransform2D()

    transitionSpeed = 666 * 0.001

end

function Update(dt)

    if (isTransitioning == true) then
        if (transform:GetMask().x > 0) then
            transform:SetMask(float2.new(transform:GetMask().x - transitionSpeed * dt, 1))
        else
            isTransitioning = false
        end
    end
end

Log("IntroEnd.lua\n")
