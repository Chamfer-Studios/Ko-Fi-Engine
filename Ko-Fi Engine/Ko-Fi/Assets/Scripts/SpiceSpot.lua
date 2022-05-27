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
end

---------------------------------------------------------

-------------------- Events -----------------------------
-- function EventHandler(key, fields)
-- end
---------------------------------------------------------

------------------ Collisions ---------------------------
function OnTriggerEnter(go)
    if (go.tag == Tag.PLAYER) then
        DispatchGlobalEvent("Spice_Reward", {spiceSpot})
        DeleteGameObject()
    end
end
---------------------------------------------------------

print("SpiceSpot.lua compiled succesfully\n")
Log("SpiceSpot.lua compiled succesfully\n")
