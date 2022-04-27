gameObject.active = false

-- Called each loop iteration
function Update(dt)
if (GetInput(4) == KEY_STATE.KEY_DOWN) then -- SPACE
	startTime = os.time()
	endTime = startTime+1
end
if os.time() >= endTime then
    gameObject.active = true
end
end

print("UI_Disable.lua compiled succesfully")