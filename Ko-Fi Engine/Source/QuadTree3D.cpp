#include "QuadTree3D.h"
#include <algorithm>
#include "C_Mesh.h"

#include "SDL_opengl.h"

QuadTreeNode3D::QuadTreeNode3D(const AABB aabb, QuadTreeNode3D* parent = nullptr) : aabb(aabb), parent(parent)
{
	for (int i = 0; i < 4; ++i)
	{
		children[i] = nullptr;
	}
}

QuadTreeNode3D::~QuadTreeNode3D()
{
	for (int i = 0; i < 4; i++)
	{
		if (children[i] != nullptr)
		{
			children[i]->objects.clear();
			delete children[i];
		}
	}
}

bool QuadTreeNode3D::IsLeaf() const
{
	return children[0] == nullptr;
}

void QuadTreeNode3D::Insert(GameObject* object)
{
	bool fits = (objects.size() < QUADTREE_MAX_ITEMS || aabb.HalfSize().LengthSq() <= QUADTREE_MIN_SIZE * QUADTREE_MIN_SIZE);
	if (IsLeaf() && fits)
		objects.push_back(object);
	else
	{
		if (IsLeaf())
		{
			Subdivide();
			objects.push_back(object);
			PutChildrenInDivisions();
		}
		else
		{
			for (int i = 0; i < 4; ++i)
			{
				if (children[i]->aabb.Intersects(object->BoundingAABB()))
					children[i]->Insert(object);
			}
		}
	}
}

void QuadTreeNode3D::Remove(GameObject* object)
{
	if (!IsLeaf())
	{
		for (int i = 0; i < 4; ++i)
			children[i]->Remove(object);
		return;
	}

	auto it = std::find(objects.begin(), objects.end(), object);

	if (it != objects.end())
		objects.erase(it);
}

void QuadTreeNode3D::Clear()
{
	for (int i = 0; i < 4; i++)
	{
		if (children[i] != nullptr)
		{
			children[i]->Clear();
		}
	}
	objects.clear();
	objects.shrink_to_fit();
}

void QuadTreeNode3D::Draw()
{
	for (int i = 0; i < 12; i++)
	{
		glVertex3f(aabb.Edge(i).a.x, aabb.Edge(i).a.y, aabb.Edge(i).a.z);
		glVertex3f(aabb.Edge(i).b.x, aabb.Edge(i).b.y, aabb.Edge(i).b.z);
	}

	for (int i = 0; i < 4; i++)
	{
		if (children[i] != nullptr)
			children[i]->Draw();
	}
}

void QuadTreeNode3D::Subdivide()
{
	float3 size = aabb.Size();
	float3 childrenSize = float3(size.x * 0.5f, size.y, size.z * 0.5f);

	float3 center = aabb.CenterPoint();
	float3 childCenter;

	AABB child;

	childCenter = float3(center.x - childrenSize.x * 0.5f, center.y, center.z - childrenSize.z * 0.5f);
	child.SetFromCenterAndSize(childCenter, childrenSize);
	children[0] = new QuadTreeNode3D(child, this);

	childCenter = float3(center.x + childrenSize.x * 0.5f, center.y, center.z - childrenSize.z * 0.5f);
	child.SetFromCenterAndSize(childCenter, childrenSize);
	children[1] = new QuadTreeNode3D(child, this);

	childCenter = float3(center.x + childrenSize.x * 0.5f, center.y, center.z + childrenSize.z * 0.5f);
	child.SetFromCenterAndSize(childCenter, childrenSize);
	children[2] = new QuadTreeNode3D(child, this);

	childCenter = float3(center.x - childrenSize.x * 0.5f, center.y, center.z + childrenSize.z * 0.5f);
	child.SetFromCenterAndSize(childCenter, childrenSize);
	children[3] = new QuadTreeNode3D(child, this);
}

void QuadTreeNode3D::PutChildrenInDivisions()
{
	for (auto it = objects.begin(); it != objects.end();)
	{
		if ((*it)->GetComponent<C_Mesh>() == nullptr) {
			it++;
			continue;
		}
		float intersecting[4] = { 0, 0, 0, 0 };
		for (int i = 0; i < 4; i++)
			intersecting[i] = children[i]->aabb.Intersection((*it)->BoundingAABB()).Volume();

		bool allIntersecting = true;
		for (int i = 0; i < 4; i++)
		{
			if (intersecting[i] <= 0)
				allIntersecting = false;
		}
		if (allIntersecting) it++;
		else
		{
			auto max = std::distance(intersecting, std::max_element(intersecting, intersecting + 4));
			if (children[max]->aabb.Contains((*it)->BoundingAABB()))
			{
				children[max]->Insert(*it);
				it = objects.erase(it);
			}
			else it++;
		}
	}
}

void QuadTreeNode3D::GetAllObjects(std::vector<GameObject*>& result)
{
	for (auto it = objects.begin(); it != objects.end(); ++it)
		result.push_back(*it);

	for (int i = 0; i < 4; i++)
	{
		if (children[i] != nullptr)
			children[i]->GetAllObjects(result);
	}
}

QuadTree3D::QuadTree3D(AABB boundaries)
{
	root = new QuadTreeNode3D(boundaries);
}

QuadTree3D::~QuadTree3D()
{
	delete root;
}

void QuadTree3D::Clear()
{
	if (root == nullptr) return;
	root->Clear();
}

void QuadTree3D::Draw()
{
	glBegin(GL_LINES);
	glLineWidth(3.0f);
	glColor3f(0.8f, 0.8f, 0.f);

	if (root != nullptr)
		root->Draw();

	glEnd();
}

void QuadTree3D::AddObjects(std::vector<GameObject*>& objects)
{
	for (auto it = objects.begin(); it != objects.end(); ++it)
	{
		if ((*it)->GetComponent<C_Mesh>() == nullptr) continue;
		Insert(*it);
		AddObjects((*it)->children);
	}
}

void QuadTree3D::Insert(GameObject* object)
{
	if (object->BoundingAABB().Intersects(root->aabb))
	{
		root->Insert(object);
	}
}

void QuadTree3D::Remove(GameObject* object)
{
	root->Remove(object);
}

void QuadTree3D::GetAllObjects(std::vector<GameObject*>& result)
{
	root->GetAllObjects(result);
}