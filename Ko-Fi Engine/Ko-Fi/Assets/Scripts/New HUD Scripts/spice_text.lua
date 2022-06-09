textSpice = nil
textTimer = nil
textDuration = 2
spiceAmount = -1

function Start()
    sizeY = gameObject:GetTransform2D():GetSize().y

    UpdateSpiceAmount()
    local digitCount = NumberOfDigits(spiceAmount)

    local newSizeX = digitCount * 17.5
    gameObject:GetTransform2D():SetSize(float2.new(newSizeX, sizeY))

    textSpice = tostring(spiceAmount)
    gameObject:GetText():SetTextValue(textSpice)
end

function Update(dt)
    if (textTimer ~= nil) then
        textTimer = textTimer + dt
        if textTimer >= textDuration then
            UpdateSpiceAmount()
            local digitCount = NumberOfDigits(spiceAmount)

            local newSizeX = digitCount * 17.5
            gameObject:GetTransform2D():SetSize(float2.new(newSizeX, sizeY))

            textSpice = tostring(spiceAmount)
            gameObject:GetText():SetTextValue(textSpice)

            textTimer = nil
        end
    end
end

function UpdateSpiceAmount()
    spiceAmount = GetVariable("GameState.lua", "spiceAmount", INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT)
end

function EventHandler(key, fields)
    if key == "Spice_Reward" then
        textTimer = 0.0
        spiceReward = fields[1]
        textSpice = spiceAmount .. " + " .. spiceReward
        gameObject:GetTransform2D():SetSize(float2.new(#textSpice * 17.5, sizeY))
        gameObject:GetText():SetTextValue(textSpice)
    elseif key == "Used_Ultimate" then
        textTimer = 0.0
        spiceCost = fields[1]
        textSpice = spiceAmount .. " - " .. spiceCost
        gameObject:GetTransform2D():SetSize(float2.new(#textSpice * 17.5, sizeY))
        gameObject:GetText():SetTextValue(textSpice)
    end
end

function NumberOfDigits(number)
    local a = number
    if a == 0 then
        return 1
    else
        local digits = 0
        while (a >= 1) do
            a = a / 10;
            digits = digits + 1;
        end
        return digits;
    end
end
