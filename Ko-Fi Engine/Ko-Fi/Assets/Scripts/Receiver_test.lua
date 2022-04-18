function EventHandler(key, fields)
    Log(key .. "\n")
    for i=1,#fields do
        Log(tostring(fields[i]) .. "\n")
    end
end