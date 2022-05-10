#include "GameObject.h"
#include "C_Transform.h"
#include "C_Script.h"

LanguageEnvironment::LanguageEnvironment(C_Script* _script)
{
	script = _script;
	gameObject = _script->owner;
	componentTransform = gameObject->GetComponent<C_Transform>();
}

LanguageEnvironment::~LanguageEnvironment()
{
}
