#pragma once

#include "MathGeoLib/MathGeoLib.h"
#include "GameObject.h"

#include <queue>
#include <vector>

#define QUADTREE_MAX_ITEMS 4
#define QUADTREE_MIN_SIZE 5.0f

class QuadTreeNode3D
{
public:
	QuadTreeNode3D(const AABB box, QuadTreeNode3D* parent);
	~QuadTreeNode3D();

	bool IsLeaf() const;

	void Insert(GameObject* object);
	void Remove(GameObject* object);

	void Clear();

	void Draw();

	void Subdivide();
	void PutChildrenInDivisions();

	template<typename CULL>
	void TestIntersection(const CULL& culler, std::queue<GameObject*>& result) const;

	void GetAllObjects(std::vector<GameObject*>& result);

public:

	AABB aabb;
	std::vector<GameObject*> objects;
	QuadTreeNode3D* parent = nullptr;
	QuadTreeNode3D* children[4];

};

class QuadTree3D
{
public:
	QuadTree3D(AABB boundaries);
	~QuadTree3D();

	void Clear();
	void AddObjects(std::vector<GameObject*>& objects);
	void Insert(GameObject* object);
	void Remove(GameObject* object);

	void Draw();

	template<typename CULL>
	void TestIntersection(const CULL& culler, std::queue<GameObject*>& result) const;

	void GetAllObjects(std::vector<GameObject*>& result);

public:
	QuadTreeNode3D* root = nullptr;
};

template<typename CULL>
void QuadTreeNode3D::TestIntersection(const CULL& culler, std::queue<GameObject*>& result) const
{
	if (!culler.Intersects(aabb))
		return;

	for (auto it = objects.begin(); it != objects.end(); ++it)
	{
		if (culler.Intersects((*it)->BoundingAABB()))
			result.push(*it);
	}

	for (int i = 0; i < 4; ++i)
	{
		if (children[i] != nullptr)
			children[i]->TestIntersection(culler, result);
	}
}

template<typename CULL>
inline void QuadTree3D::TestIntersection(const CULL& culler, std::queue<GameObject*>& result) const
{
	if (root != nullptr)
		root->TestIntersection(culler, result);
}