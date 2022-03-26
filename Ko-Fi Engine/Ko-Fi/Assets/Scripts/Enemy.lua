State = {
    PATROL = 1,
    SEEK = 2
}

currentState = State.PATROL
speed = 20

local speedIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT
speedIV = InspectorVariable.new("speed", speedIVT, speed)
NewVariable(speedIV)

waypoints = {}

local waypointsIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_FLOAT3_ARRAY
waypointsIV = InspectorVariable.new("waypoints", waypointsIVT, waypoints)
NewVariable(waypointsIV)

function Update(dt)
    Log(tostring(#waypoints))
    for i=1,#waypoints do
        Log(tostring(i))
        Log(tostring(waypoints[-1]))
    end
end