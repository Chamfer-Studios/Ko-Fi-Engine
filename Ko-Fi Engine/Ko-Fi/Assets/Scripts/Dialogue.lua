--------------------------------------------
-- Class Attributes --
--------------------------------------------
_name = 10
local nameIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_INT			-- IVT == Inspector Variable Type
local nameIV = InspectorVariable.new("name", nameIVT, _name)
NewVariable(nameIV)

Dialogue = {}
MetaDialgoue = {}
MetaDialgoue.__index = Dialogue
--------------------------------------------
-- Class Methods--
--------------------------------------------

function Dialgoue.new(name)
	local instance = setmetatable({}, MetaDialgoue)
	instance.name = name
	return instance
end

function Dialogue:displayName()
print(name)
end

function Dialogue:printInspectorValues()

end

function Update(dt)

print(Dialogue.name)
end


--------------------------------------------
-- Class Constructor --
--------------------------------------------

function Dialogue:new(t)
	t = t or {}
	setmetatable(t, self)
	self.__index = self
	return t 
end