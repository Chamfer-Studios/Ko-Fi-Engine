filter = "terrain"

-- children[1]:AddComponentByType(ComponentType.BOX_COLLIDER)

function Start()
    children = gameObject:GetChildren()

    for i = 1, #children do
        children[i]:AddComponentByType(ComponentType.BOX_COLLIDER)
        children[i]:GetRigidBody():SetStatic()
        children[i]:GetBoxCollider():SetFilter(filter)
        children[i]:GetBoxCollider():UpdateFilter()
    end
end

Log("SceneCollisions.lua\n")
