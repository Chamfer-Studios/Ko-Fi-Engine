spiceAmount = 0
spiceMaxLvl1 = 10000

defaultSpiceBarPath = "Assets/New UI/spice_bar_fill_default.png"
isDefaultBar = true

-- Taken spice
takenSpice = false
whiteBlinkTimer = nil
whiteBlinkTotalTimer = nil
whiteSpiceBarPath = "Assets/New UI/spice_bar_fill_white.png"
whiteBlinkDuration = 0.9
whiteBlinkTransitionSpeed = 0.2

-- Not enough spice
notEnoughSpice = false
redBlinkTimer = nil
redBlinkTotalTimer = nil
redSpiceBarPath = "Assets/New UI/spice_bar_fill_red.png"
redBlinkDuration = 0.9
redBlinkTransitionSpeed = 0.2

function Start()
    spiceAmount = GetVariable("GameState.lua", "spiceAmount", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
    spiceMaxLvl1 = GetVariable("GameState.lua", "spiceMaxLvl1", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
    maskValue = spiceAmount / spiceMaxLvl1

    gameObject:GetTransform2D():SetMask(float2.new(maskValue, 1))
end

function Update(dt)

    spiceAmount = GetVariable("GameState.lua", "spiceAmount", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
    if (spiceAmount > spiceMaxLvl1) then
        spiceAmount = spiceMaxLvl1
        maskValue = spiceAmount / spiceMaxLvl1
        gameObject:GetTransform2D():SetMask(float2.new(maskValue, 1))
    end
    maskValue = spiceAmount / spiceMaxLvl1
    gameObject:GetTransform2D():SetMask(float2.new(maskValue, 1))

    if takenSpice == true then
        TakenSpice(dt)
    elseif notEnoughSpice == true then
        NotEnoughSpice(dt)
    end
end

function TakenSpice(dt)
    whiteBlinkTimer = whiteBlinkTimer + dt
    whiteBlinkTotalTimer = whiteBlinkTotalTimer + dt
    if whiteBlinkTimer >= whiteBlinkTransitionSpeed then
        if isDefaultBar == true then
            isDefaultBar = false
            gameObject:GetImage():SetTexture(whiteSpiceBarPath)
            whiteBlinkTimer = 0.1
        else
            isDefaultBar = true
            gameObject:GetImage():SetTexture(defaultSpiceBarPath)
            whiteBlinkTimer = 0.0
        end
    end

    if whiteBlinkTotalTimer >= whiteBlinkDuration + whiteBlinkTransitionSpeed then
        gameObject:GetImage():SetTexture(defaultSpiceBarPath)
        whiteBlinkTimer = nil
        takenSpice = false
        isDefaultBar = true
    end
end

function NotEnoughSpice(dt)
    redBlinkTimer = redBlinkTimer + dt
    redBlinkTotalTimer = redBlinkTotalTimer + dt
    if redBlinkTimer >= redBlinkTransitionSpeed then
        if isDefaultBar == true then
            isDefaultBar = false
            gameObject:GetImage():SetTexture(redSpiceBarPath)
            redBlinkTimer = 0.1
        else
            isDefaultBar = true
            gameObject:GetImage():SetTexture(defaultSpiceBarPath)
            redBlinkTimer = 0.0
        end
    end

    if redBlinkTotalTimer >= redBlinkDuration + redBlinkTransitionSpeed then
        gameObject:GetImage():SetTexture(defaultSpiceBarPath)
        redBlinkTimer = nil
        notEnoughSpice = false
        isDefaultBar = true
    end
end

function EventHandler(key, fields)
    if key == "Spice_Reward" then
        takenSpice = true
        whiteBlinkTimer = 0.0
        whiteBlinkTotalTimer = 0.0
    elseif key == "Not_Enough_Spice" then
        notEnoughSpice = true
        redBlinkTimer = 0.0
        redBlinkTotalTimer = 0.0
    end
end
