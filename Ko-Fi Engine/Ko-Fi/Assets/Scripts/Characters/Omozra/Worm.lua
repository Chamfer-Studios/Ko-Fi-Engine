State = {
	IDLE = 1,
	EAT = 2,
	AWAIT = 3,
	SPIT = 4,
	DEVOUR = 5,
}

------------------- Variables --------------------

target = nil
currentState = State.IDLE

-------------------- Methods ---------------------

function Start()

	omozra = GetVariable("Omozra.lua", "gameObject", INSPECTOR_VARIABLE_TYPE.INSPECTOR_GAMEOBJECT)
	componentTransform:SetPosition(float3.new(omozra:GetTransform():GetPosition().x, -20, omozra:GetTransform():GetPosition().z))

    particles = gameObject:GetComponentParticle()
    particles:StopParticleSpawn()

    componentAnimator = gameObject:GetComponentAnimator()
    if (componentAnimator ~= nil) then
		componentAnimator:SetSelectedClip("Idle")
	end
end

-- Called each loop iteration
function Update(dt)
	
	-- Animation timer
	if (componentAnimator ~= nil) then
		if (componentAnimator:IsCurrentClipLooping() == false) then
			if (componentAnimator:IsCurrentClipPlaying() == false) then
                if (currentState == State.EAT) then
                    currentState = State.AWAIT
				elseif (currentState == State.SPIT) then
					DeleteGameObject()
                end
			end
		end
	end
end

-------------------- Events ----------------------
function EventHandler(key, fields)
	
	if (key == "Worm_State") then -- fields[1] -> state, fields[2] -> target;

		if (fields[1] == State.DEVOUR) then
			currentState = State.DEVOUR
			target = fields[2]
			componentTransform:SetPosition(float3.new(target:GetTransform():GetPosition().x, -20, target:GetTransform():GetPosition().z))
			if (componentAnimator ~= nil) then
				componentAnimator:SetSelectedClip("Devour")
			end
			if (particles ~= nil)then
				particles:ResumeParticleSpawn()
			end
		end
	elseif (key == "Omozra_Ultimate_Recast") then -- fields[1] -> go;
		
        componentTransform:SetPosition(fields[1])
        currentState = State.SPIT
        componentAnimator:SetSelectedClip("Spit")
    end
end
--------------------------------------------------

print("Worm.lua compiled succesfully")