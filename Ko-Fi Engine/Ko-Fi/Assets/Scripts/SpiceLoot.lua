------------------- Variables setter --------------------
spiceLoot = 100
---------------------------------------------------------

------------------- Inspector setter --------------------
spiceLootIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
spiceLootIV = InspectorVariable.new("spiceLoot", spiceLootIVT, spiceLoot)
NewVariable(spiceLootIV)
---------------------------------------------------------

----------------------- Methods -------------------------
function Start()
    boxCollider = gameObject:GetBoxCollider()
    componentRigidBody = gameObject:GetRigidBody()

    DispatchGlobalEvent("Spice_Has_Spawned", {})
end

---------------------------------------------------------

function Update(dt)

end

-------------------- Events -----------------------------
function EventHandler(key, fields)
    if key == "Spice_Drop" then
        gameObject:GetTransform():SetPosition(float3.new(fields[1], fields[2], fields[3]))

        enemyType = fields[4]
        if (enemyType == "Harkonnen") then
            math.randomseed(os.time())
            spiceLoot = math.random(40, 80)
        elseif (enemyType == "Sardaukar") then
            math.randomseed(os.time())
            spiceLoot = math.random(80, 160)
        elseif (enemyType == "Mentat") then
            math.randomseed(os.time())
            spiceLoot = math.random(100, 200)
        end
    end
end
---------------------------------------------------------

------------------ Collisions ---------------------------
function OnTriggerEnter(go)
    if go.tag == Tag.PLAYER then
        DeleteGameObject()
        DispatchGlobalEvent("Spice_Reward", {spiceLoot})
    end
end
---------------------------------------------------------

print("SpiceLoot.lua compiled succesfully")
Log("SpiceLoot.lua compiled succesfully\n")
