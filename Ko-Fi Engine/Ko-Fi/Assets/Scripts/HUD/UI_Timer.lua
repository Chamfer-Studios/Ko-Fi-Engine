gameObject.active = false
msec = 9
sec = 5
min = 2
stopTimer = false
-- Called each loop iteration
function Update(dt)
	if (stopTimer == false) then
		if (GetInput(4) == KEY_STATE.KEY_DOWN) then -- SPACE
			startTime = os.time()
			endTime = startTime+1
		end
		if (os.time() >= endTime) then
			gameObject.active = true
			gameObject:GetText():SetTextValue(min .. ":" .. sec .. msec)
			if (min == 0) then
				if (sec == 0) then
					if (msec == 0) then
						stopTimer = true
					end
				end
			end
			if (msec == 0) then
				msec = 10
				if (sec == 0) then
					sec = 6
					min = min - 1
				end
				sec = sec - 1
			end
			msec = msec - 1
			startTime = os.time()
			endTime = startTime+1
		end
	end
end

print("UI_Timer.lua compiled succesfully")