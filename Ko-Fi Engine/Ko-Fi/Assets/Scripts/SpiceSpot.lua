------------------- Variables setter --------------------
spiceSpot = 1000
---------------------------------------------------------

------------------- Inspector setter --------------------
spiceSpotIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
spiceSpotIV = InspectorVariable.new("spiceSpot", spiceSpotIVT, spiceSpot)
NewVariable(spiceSpotIV)
---------------------------------------------------------

----------------------- Methods -------------------------
function Start()
    boxCollider = gameObject:GetBoxCollider()
	componentRigidBody = gameObject:GetRigidBody()
end

---------------------------------------------------------

-------------------- Events -----------------------------
-- function EventHandler(key, fields)
-- end
---------------------------------------------------------

------------------ Collisions ---------------------------
function OnTriggerEnter(go)
    if (go.tag == Tag.PLAYER) then
        DeleteGameObject()
        DispatchGlobalEvent("Spice_Reward", {spiceSpot})
    end
end
---------------------------------------------------------

print("SpiceSpot.lua compiled succesfully")
Log("SpiceSpot.lua compiled succesfully\n")