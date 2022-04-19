------------------- Variables --------------------

lifeTime = 10.0	-- secs --iv required
lifeTimer = 0
effectRadius = 250.0
componentRigidBody = gameObject:GetRigidBody()

-------------------- Methods ---------------------

-- Called each loop iteration
function Update(dt)
	
	if (lifeTimer <= lifeTime) then
		
		
		lifeTimer = lifeTimer + dt
	else
		DeleteGameObject()
	end
end

--------------------------------------------------

print("Decoy.lua compiled succesfully")