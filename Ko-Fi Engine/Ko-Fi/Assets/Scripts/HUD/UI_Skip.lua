currentCredits = 1
-- Called each loop iteration
function Update(dt)
	if (gameObject.active == true) then
		if (currentCredits == 1) then
            gameObject:GetChild("Image2").active = true
            gameObject:GetChild("Image3").active = false
            gameObject:GetChild("Image4").active = false
            gameObject:GetChild("Image5").active = false
            gameObject:GetChild("Image6").active = false
        elseif (currentCredits == 2) then
            gameObject:GetChild("Image2").active = false
            gameObject:GetChild("Image3").active = true
            gameObject:GetChild("Image4").active = false
            gameObject:GetChild("Image5").active = false
            gameObject:GetChild("Image6").active = false
        elseif (currentCredits == 3) then
            gameObject:GetChild("Image2").active = false
            gameObject:GetChild("Image3").active = false
            gameObject:GetChild("Image4").active = true
            gameObject:GetChild("Image5").active = false
            gameObject:GetChild("Image6").active = false
        elseif (currentCredits == 4) then
            gameObject:GetChild("Image2").active = false
            gameObject:GetChild("Image3").active = false
            gameObject:GetChild("Image4").active = false
            gameObject:GetChild("Image5").active = true
            gameObject:GetChild("Image6").active = false
        elseif (currentCredits == 5) then
            gameObject:GetChild("Image2").active = false
            gameObject:GetChild("Image3").active = false
            gameObject:GetChild("Image4").active = false
            gameObject:GetChild("Image5").active = false
            gameObject:GetChild("Image6").active = true
        end
    end
end

print("UI_Skip.lua compiled succesfully")