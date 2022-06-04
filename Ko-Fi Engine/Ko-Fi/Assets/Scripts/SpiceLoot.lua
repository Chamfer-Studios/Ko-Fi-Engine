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
    DispatchGlobalEvent("Spice_Has_Spawned", {})
end

---------------------------------------------------------

function Update(dt)

end

-------------------- Events -----------------------------
function EventHandler(key, fields)
    if key == "Spice_Drop" then
        math.randomseed(os.time())
        randomOffsetX = 10
        randomOffsetZ = 10
        rngX = math.random(-randomOffsetX, randomOffsetX)
        rngZ = math.random(-randomOffsetZ, randomOffsetZ)
        -- Log ("Spawn loot offset X: " .. rngX .. " offsetZ: " .. rngZ .. "\n")
        gameObject:GetTransform():SetPosition(float3.new(fields[1] + rngX, fields[2], fields[3] + rngZ))

        enemyType = fields[4]
        if (enemyType == "Harkonnen") then
            spiceLoot = math.random(40, 80)
        elseif (enemyType == "Sardaukar") then
            spiceLoot = math.random(80, 160)
        elseif (enemyType == "Mentat") then
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
