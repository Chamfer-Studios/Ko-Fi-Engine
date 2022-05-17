function EventHandler(key, fields)
    if key == "FOW_WriteCircle" then
        gameObject:GetFOW():WriteCircle(fields[1], fields[2], fields[3])
    end
end