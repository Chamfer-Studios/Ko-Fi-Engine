#ifndef __MODULE_H__
#define __MODULE_H__


#include "SDL_events.h"

#include "JsonHandler.h"
#include "Event.h"
#include <list>

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

	virtual void OnGui() {}

	virtual void OnClick(SDL_Event event) {}
	// Methods to manage observers
	void AddObserver(Module* observer)
	{
		observers.push_back(observer);
	}

	void RemoveObserver(Module* observer)
	{
		observers.remove(observer);
	}

	// Method to receive and manage events
	virtual void OnNotify(const Event& event) = 0;

protected:
	// Method to notify events to other modules in order to manage them
	void notify(const Event& event)
	{
		for (std::list<Module*>::iterator observer = observers.begin(); observer != observers.end(); observer++)
		{
			(*observer)->OnNotify(event);
		}
	}

public:
	std::string name;
	bool active;

private:
	// To keep track of the observers of each module
	std::list<Module*> observers;
	int numObservers;
};

#endif // !__MODULE_H__