isStarting = true

function Update(dt)
    if (isStarting == true) then
        timer = 0
        step = 1
        children = gameObject:GetChildren()
		for i = 1, #children do
            if(i == 1)then
                children[i]:Active(true)    
            else
                children[i]:Active(false)   
            end
        end
        isStarting = false
	end
    timer = timer + dt;
    --str = "Timer value:" .. timer .. "\n"
    --Log(str)

    if(timer > 0.1) then
        if(step == 1 or step == 7)  then
            children[6]:Active(false)
            children[1]:Active(true)
            step = 2
        else
            StepAnimation(step)
            step = step + 1
        end
        timer = 0;
    end
end


function StepAnimation(step)
        children[step - 1]:Active(false)
        children[step]:Active(true)
end