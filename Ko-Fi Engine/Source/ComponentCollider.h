#ifndef __COMPONENT_COLLIDER_H__
#define __COMPONENT_COLLIDER_H__

#include "Component.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/Quat.h"

class ComponentCollider : public Component
{
public:
	enum class COLLIDER_TYPE
	{
		COLL_SPHERE,
		COLL_PLANE,
		COLL_CAPSULE,
		COLL_BOX
	};

	ComponentCollider(GameObject* parent);
	~ComponentCollider();

	

	// ----- Editor settings ---------------
	bool InspectorDraw(PanelChooser* chooser);
	// -------------------------------------

	// ----- Serialization -----------------

	// -------------------------------------

private:

};


#endif
