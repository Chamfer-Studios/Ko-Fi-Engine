#include "Resource.h"

Resource::Resource(UID uid, Resource::Type type, const char* assetPath) : uid(uid), type(type), assetPath(assetPath) 
{

};

Resource::~Resource()
{

}

bool Resource::HasResource(UID uid) const
{
	return false;
}