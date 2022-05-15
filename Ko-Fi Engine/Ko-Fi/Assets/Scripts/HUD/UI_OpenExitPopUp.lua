name = "ExitMenu"
popUp = true

local nameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_STRING
nameIV = InspectorVariable.new("name", nameIVT, name)
NewVariable(nameIV)

function Start()
	if (GetVariable("UI_OpenExitPopUp.lua", "popUp", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == false) then
		gameObject:Active(false)
	elseif(GetVariable("UI_OpenExitPopUp.lua", "popUp", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == true) then
		gameObject:Active(true)
	end
end

-- Called each loop iteration
function Update(dt)
	if (GetInput(25) == KEY_STATE.KEY_DOWN) then -- ESC
		Log("ESC Pressed")
		if (popUp == true) then
			child = gameObject
			Log(child:GetName())
			child:Active(false)
			popUp = false
		elseif (popUp == false) then
			popUp = true
			child:Active(true)
		end
	end
end

print("UI_OpenCreditsPopUp.lua compiled succesfully")