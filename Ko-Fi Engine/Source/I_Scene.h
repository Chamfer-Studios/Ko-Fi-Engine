#ifndef __I_SCENE__
#define __I_SCENE__

class aiScene;
class Scene;

namespace Importer
{
	namespace Scenes
	{
		bool ImportOne(const aiScene* aiScene, Scene* scene);
		bool ImportMultiple(const aiScene* aiScene, Scene* scene);
		bool Save(const Scene* mesh, const char* path);
		bool Load(const char* path, Scene* mesh);
	}
}

#endif // !__I_SCENE__