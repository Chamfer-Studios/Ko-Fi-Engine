isSkill = false
isQuest = false

local isSkillIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL
isSkillIV = InspectorVariable.new("isSkill", isSkillIVT, isSkill)
NewVariable(isSkillIV)

local isQuestIVT = INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL
isQuestIV = InspectorVariable.new("isQuest", isQuestIVT, isQuest)
NewVariable(isQuestIV)

-- Called each loop iteration
function Update(dt)
	if (GetVariable("UI_OpenSkillsPopUp.lua", "popUp", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == isSkill) then
		if (GetVariable("UI_OpenQuestsPopUp.lua", "popUp", INSPECTOR_VARIABLE_TYPE.INSPECTOR_BOOL) == isQuest) then
			if (gameObject:GetButton():IsPressed() == true) then
				gameObject:GetAudioSwitch():PlayTrack(0)
			end
			if (gameObject:GetButton():IsHovered() == true) then
				gameObject:GetAudioSwitch():PlayTrack(1)
			end
		end
	end
end

print("UI_SFXTS.lua compiled succesfully")