gameObject.active = false

-- Called each loop iteration
function Update(dt)
	mouseX = gameObject:GetButton():GetMouseX()
	print("x pos:" .. mouseX)
end

print("UI_ItsActuallyASlider.lua compiled succesfully")