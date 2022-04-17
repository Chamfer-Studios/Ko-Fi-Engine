function Update()
    camera = GetCamera()
    position = componentTransform:GetPosition()
    pos = camera:WorldToScreen(position)
    Log(tostring(pos) .. "\n")
end