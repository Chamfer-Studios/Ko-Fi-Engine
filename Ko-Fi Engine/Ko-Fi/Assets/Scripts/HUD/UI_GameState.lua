spice = 0

zhib_available = true
zhib_primary_level = 0
zhib_secondary_level = 0
zhib_ultimate_level = 0
zhib_passive_level = 0

nerala_available = true
nerala_primary_level = 0
nerala_secondary_level = 0
nerala_ultimate_level = 0
nerala_passive_level = 0

omozra_available = true
omozra_primary_level = 0
omozra_secondary_level = 0
omozra_ultimate_level = 0
omozra_passive_level = 0

function Start()
    LoadGameState()

    spice = GetGameJsonInt("spice")

    zhib_available = GetGameJsonBool("zhib_available")
    zhib_primary_level = GetGameJsonInt("zhib_primary_level")
    zhib_secondary_level = GetGameJsonInt("zhib_secondary_level")
    zhib_ultimate_level = GetGameJsonInt("zhib_ultimate_level")
    zhib_passive_level = GetGameJsonInt("zhib_passive_level")

    nerala_available = GetGameJsonBool("nerala_available")
    nerala_primary_level = GetGameJsonInt("nerala_primary_level")
    nerala_secondary_level = GetGameJsonInt("nerala_secondary_level")
    nerala_ultimate_level = GetGameJsonInt("nerala_ultimate_level")
    nerala_passive_level = GetGameJsonInt("nerala_passive_level")
    
    omozra_available = GetGameJsonBool("omozra_available")
    omozra_primary_level = GetGameJsonInt("omozra_primary_level")
    omozra_secondary_level = GetGameJsonInt("omozra_secondary_level")
    omozra_ultimate_level = GetGameJsonInt("omozra_ultimate_level")
    omozra_passive_level = GetGameJsonInt("omozra_passive_level")

    DispatchEvent("Gamestate_Loaded", {})
end

function EventHandler(key, fields)
    if key == "Save_Game" then

        SetGameJsonInt("spice", spice)

        SetGameJsonInt("nerala_primary_level", nerala_primary_level)
        SetGameJsonInt("nerala_secondary_level", nerala_secondary_level)
        SetGameJsonInt("nerala_ultimate_level", nerala_ultimate_level)
        SetGameJsonInt("nerala_passive_level", nerala_passive_level)

        SetGameJsonInt("zhib_primary_level", zhib_primary_level)
        SetGameJsonInt("zhib_secondary_level", zhib_secondary_level)
        SetGameJsonInt("zhib_ultimate_level", zhib_ultimate_level)
        SetGameJsonInt("zhib_passive_level", zhib_passive_level)

        SetGameJsonInt("omozra_primary_level", omozra_primary_level)
        SetGameJsonInt("omozra_secondary_level", omozra_secondary_level)
        SetGameJsonInt("omozra_ultimate_level", omozra_ultimate_level)
        SetGameJsonInt("omozra_passive_level", omozra_passive_level)

        SaveGameState()
    end
end

print("UI_GameState.lua compiled successfully!")
Log("UI_GameState.lua compiled successfully!\n")