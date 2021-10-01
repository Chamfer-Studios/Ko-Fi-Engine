#ifndef __MODULE_H__
#define __MODULE_H__

#include "SString.h"

#include "JsonHandler.h"

class GuiControl;
enum class GuiControlState;
class KoFiEngine;
class Module
{
public:

	Module() : active(false)
	{}

	void Init()
	{
		active = true;
	}

	// Called before render is available
	virtual bool Awake(Json configModule)
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate(float dt)
	{
		return true;
	}
	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate(float dt)
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

public:

	SString name;
	bool active;

};

#endif // __MODULE_H__