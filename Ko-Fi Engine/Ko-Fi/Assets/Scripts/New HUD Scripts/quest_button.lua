isOpened = false
function Update(dt)
    if gameObject:GetButton():IsPressed() == true then
        isOpened = not isOpened
    else
    end

    if isOpened == true then
        gameObject:GetTransform2D():SetPosition(float2.new(196, -363))
    else
        gameObject:GetTransform2D():SetPosition(float2.new(-95, -363))
    end
end
