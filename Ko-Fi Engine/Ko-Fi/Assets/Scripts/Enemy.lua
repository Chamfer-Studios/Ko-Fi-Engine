State = {
    PATROL = 1,
    SEEK = 2
}

currentState = State.PATROL
speed = 20

local speedIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
speedIV = InspectorVariable.new("speed", speedIVT, speed)
NewVariable(speedIV)

oldWaypoints = {}
waypoints = {}

local waypointsIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT3_ARRAY
waypointsIV = InspectorVariable.new("waypoints", waypointsIVT, waypoints)
NewVariable(waypointsIV)

function Update(dt)
    eq = true

    if #waypoints ~= #oldWaypoints then
        eq = false
    end

    if eq == false then
        for i=1,#waypoints do
            for j=1,3 do
                oldWaypoints[i] = {}
                oldWaypoints[i][j] = waypoints[i][j]
            end
        end
    else
        for i=1,#waypoints do
            for j=1,3 do
                if waypoints[i][j] ~= oldWaypoints[i][j] then
                    eq = false
                    oldWaypoints[i][j] = waypoints[i][j]
                end
            end
        end
    end

    if eq == false then
        Log("Need to update waypoints\n")
    end
end