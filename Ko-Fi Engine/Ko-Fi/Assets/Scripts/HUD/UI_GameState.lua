spice = 0

nerala_primary_level = 0
nerala_secondary_level = 0
nerala_ultimate_level = 0

zhib_primary_level = 0
zhib_secondary_level = 0
zhib_ultimate_level = 0

omozra_primary_level = 0
omozra_secondary_level = 0
omozra_ultimate_level = 0

function Start()
    LoadGameState()

    spice = GetGameJsonInt("spice")

    nerala_primary_level = GetGameJsonInt("nerala_primary_level")
    nerala_secondary_level = GetGameJsonInt("nerala_secondary_level")
    nerala_ultimate_level = GetGameJsonInt("nerala_ultimate_level")

    zhib_primary_level = GetGameJsonInt("zhib_primary_level")
    zhib_secondary_level = GetGameJsonInt("zhib_secondary_level")
    zhib_ultimate_level = GetGameJsonInt("zhib_ultimate_level")
    
    omozra_primary_level = GetGameJsonInt("omozra_primary_level")
    omozra_secondary_level = GetGameJsonInt("omozra_secondary_level")
    omozra_ultimate_level = GetGameJsonInt("omozra_ultimate_level")
end

function EventHandler(key, fields)
    if key == "Save_Game" then
        SetGameJsonInt("spice", spice)

        SetGameJsonInt("nerala_primary_level", nerala_primary_level)
        SetGameJsonInt("nerala_secondary_level", nerala_secondary_level)
        SetGameJsonInt("nerala_ultimate_level", nerala_ultimate_level)

        SetGameJsonInt("zhib_primary_level", zhib_primary_level)
        SetGameJsonInt("zhib_secondary_level", zhib_secondary_level)
        SetGameJsonInt("zhib_ultimate_level", zhib_ultimate_level)

        SetGameJsonInt("omozra_primary_level", omozra_primary_level)
        SetGameJsonInt("omozra_secondary_level", omozra_secondary_level)
        SetGameJsonInt("omozra_ultimate_level", omozra_ultimate_level)

        SaveGameState()
    end
end

print("UI_GameState.lua compiled succesfully")
Log("UI_GameState.lua compiled succesfully\n")