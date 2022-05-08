function Update(dt)
    DispatchGlobalEvent("Player_Position", { componentTransform:GetPosition(), gameObject })
end