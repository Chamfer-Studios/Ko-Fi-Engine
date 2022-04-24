#ifndef __M_PHYSICS_H__
#define __M_PHYSICS_H__
#include "Module.h"
#include "Globals.h"
#include "reactphysics3d/reactphysics3d.h"
#include <map>
#include <string>


class M_Physics : public Module
{
public:
	//Constructor
	M_Physics(KoFiEngine* engine);

	//Destructor
	~M_Physics();

	//Called before render is avilable
	bool Awake(Json configModule);
	bool Start();

	bool Update(float dt);
	bool RenderPhysics();
	//Called before quitting
	bool CleanUp();

	//Engine Config Serialization ----------------------------------
	bool SaveConfiguration(Json& configModule) const override;
	bool LoadConfiguration(Json& configModule) override;
	//---------------------------------------------------------------
	
	//Engine Config Inspector draw ----------------------------------
	bool InspectorDraw() override;
	// --------------------------------------------------------------

	// Method to receive and manage events
	void OnNotify(const Event& event);

	//Getters and Setters
	inline reactphysics3d::PhysicsWorld* GetWorld() { return this->world; }
	inline reactphysics3d::PhysicsCommon& GetPhysicsCommon() { return this->physicsCommon; }

	//Filters
	void AddFilter(std::string newFilter);
	void RemoveFilter(std::string filterToRemove);
	unsigned int GetFilter(std::string filter);
	inline std::map<unsigned int, std::string> GetFiltersMap() { return filters; }
private:
	// Filter matrix private methods
	inline void DeleteFilterMatrix()
	{
		for (int i = 0; i < filters.size(); i++)
			delete[] filterMatrix[i];				// To delete the inner arrays
		delete[] filterMatrix;						// To delete the outer array, which contained the pointers of all the inner arrays
	}
	inline void DeclareFilterMatrix()
	{
		size_t filSize = filters.size();													// Dimensions of the array
		filterMatrix = new bool* [filSize];												// Declare a memory block of bools of filters size
		for (int i = 0; i < filSize; ++i) { filterMatrix[i] = new bool[filSize]; }		// Declare a memory block of bools of filters size
	};
	inline void SetFilterMatrix(bool** newFilterMatrix) { filterMatrix = newFilterMatrix; }

private:
	KoFiEngine* engine = nullptr;
public:
	//DynamicsWorld* world = nullptr;

	reactphysics3d::PhysicsCommon physicsCommon;
	reactphysics3d::PhysicsWorld* world = nullptr;
	std::map<unsigned int,std::string> filters;
	std::string imguiNewFilterText;
	bool** filterMatrix = nullptr;
};

#endif // !__M_PHYSICS_H__
