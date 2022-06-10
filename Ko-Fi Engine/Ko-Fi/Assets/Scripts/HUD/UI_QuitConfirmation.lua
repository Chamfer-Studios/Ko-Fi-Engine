name = "UI_QuitConfirmation.lua"
exitPopUp = nil

function Start()
	exitPopUp = Find("ExitPopUp")
	if(exitPopUp ~= nil) then
		Log("aaaaaaaaaaaaaaaaaaa\n")
	end
end
-- Called each loop iteration
function UpdateUI(dt)
	if (gameObject:GetButton():IsPressed() == true) then
		Log("paranormal")
		if (exitPopUp.active == true) then
			exitPopUp:Active(false)
		else
			exitPopUp:Active(true)
		end
	end
end

print("UI_ClosePopUp.lua compiled succesfully")