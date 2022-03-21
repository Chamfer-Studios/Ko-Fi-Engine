#include "Mesh.h"
#include "Texture.h"

#include "GameObject.h"
#include "ComponentMaterial.h"
#include "ComponentTransform.h"
#include "ComponentAnimator.h"

#include "AnimatorClip.h"

#include "Globals.h"

#include "Importer.h"
//#include "I_Mesh.h"

// OpenGL / GLEW
#include "glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include <iostream>

Mesh::Mesh(Shape shape)
{
	verticesSizeBytes = 0;
	normalsSizeBytes = 0;
	texCoordSizeBytes = 0;
	indicesSizeBytes = 0;
	VAO = 0;
	isAnimated = false;
	meshType = shape;

	switch (shape)
	{
	case Shape::CUBE:
		PrimitiveMesh(par_shapes_create_cube());
		break;
	case Shape::CYLINDER:
		PrimitiveMesh(par_shapes_create_cylinder(20, 20));
		break;
	case Shape::SPHERE:
		PrimitiveMesh(par_shapes_create_parametric_sphere(20, 20));
		break;
	case Shape::TORUS:
		PrimitiveMesh(par_shapes_create_torus(20, 20, 0.2));
		break;
	case Shape::PLANE:
		PrimitiveMesh(par_shapes_create_plane(20, 20));
		break;
	case Shape::CONE:
		PrimitiveMesh(par_shapes_create_cone(20, 20));
		break;
	default:
		break;
	}
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &VAO);

	// Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndex);
	glDeleteBuffers(1, &idIndex);
	RELEASE_MALLOC(indices);

	// Vertices
	glBindBuffer(GL_ARRAY_BUFFER, idVertex);
	glDeleteBuffers(1, &idVertex);
	RELEASE_MALLOC(vertices);

	// Normals
	glBindBuffer(GL_ARRAY_BUFFER, idNormal);
	glDeleteBuffers(1, &idNormal);
	RELEASE_MALLOC(normals);

	// Texture coords
	glBindBuffer(GL_ARRAY_BUFFER, idTexCoord);
	glDeleteBuffers(1, &idTexCoord);
	RELEASE_MALLOC(texCoords);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::SetUpMeshBuffers()
{
	// Vertex Array Object (VAO)
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Indices
	glGenBuffers(1, &idIndex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSizeBytes, indices, GL_DYNAMIC_DRAW);

	// Vertices
	glGenBuffers(1, &idVertex);
	glBindBuffer(GL_ARRAY_BUFFER, idVertex);
	glBufferData(GL_ARRAY_BUFFER, verticesSizeBytes, vertices, GL_DYNAMIC_DRAW);
	// Add vertex position attribute to the vertex array object (VAO)
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(POSITION_LOCATION);

	// Normals
	glGenBuffers(1, &idNormal);
	glBindBuffer(GL_ARRAY_BUFFER, idNormal);
	glBufferData(GL_ARRAY_BUFFER, normalsSizeBytes, normals, GL_DYNAMIC_DRAW);
	// Add normals attribute to the vertex array object (VAO)
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(NORMAL_LOCATION);

	// Texture coords
	if (texCoords)
	{
		glGenBuffers(1, &idTexCoord);
		glBindBuffer(GL_ARRAY_BUFFER, idTexCoord);
		glBufferData(GL_ARRAY_BUFFER, texCoordSizeBytes, texCoords, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(TEX_COORD_LOCATION);
	}
	if (bones.size() > 0)
	{
		glGenBuffers(1, &idBones);
		glBindBuffer(GL_ARRAY_BUFFER, idBones);
		glBufferData(GL_ARRAY_BUFFER, sizeof(bones[0]) * bones.size(), &bones[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(BONE_LOCATION);
		glVertexAttribPointer(BONE_LOCATION, MAX_NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (void*)0);
		glEnableVertexAttribArray(WEIGHT_LOCATION);
		glVertexAttribPointer(WEIGHT_LOCATION, MAX_NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)(MAX_NUM_BONES_PER_VERTEX * sizeof(int32_t)));
	}
	
	// Unbind any vertex array we have binded before.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::Draw()
{
	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, indicesSizeBytes / sizeof(uint), GL_UNSIGNED_INT, NULL);

	DebugDraw();

	glBindVertexArray(0);

	// Unbind Texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Mesh::DebugDraw()
{
	// Debug draw
	if (drawVertexNormals)
		DrawVertexNormals();

	if (drawFaceNormals)
		DrawFaceNormals();
}

void Mesh::DrawVertexNormals() const
{
	if (idNormal == -1 || normals == nullptr)
		return;

	float normal_lenght = 0.5f;

	// Vertices normals
	glBegin(GL_LINES);
	for (size_t i = 0, c = 0; i < verticesSizeBytes / sizeof(float); i += 3, c += 4)
	{
		glColor3f(1.0f, 0.0f, 0.0f);
		//glColor4f(colors[c], colors[c + 1], colors[c + 2], colors[c + 3]);
		glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);

		glVertex3f(vertices[i] + (normals[i] * normal_lenght),
			vertices[i + 1] + (normals[i + 1] * normal_lenght),
			vertices[i + 2] + (normals[i + 2]) * normal_lenght);
	}

	glColor3f(1.0f, 1.0f, 1.0f);
	glEnd();
}

void Mesh::DrawFaceNormals() const
{
	if (idNormal == -1 || normals == nullptr)
		return;

	// Face normals
	glBegin(GL_LINES);
	for (size_t i = 0; i < verticesSizeBytes / sizeof(float); i += 3)
	{
		glColor3f(0.0f, 1.0f, 0.0f);
		float vx = (vertices[i] + vertices[i + 3] + vertices[i + 6]) / 3;
		float vy = (vertices[i + 1] + vertices[i + 4] + vertices[i + 7]) / 3;
		float vz = (vertices[i + 2] + vertices[i + 5] + vertices[i + 8]) / 3;

		float nx = (normals[i] + normals[i + 3] + normals[i + 6]) / 3;
		float ny = (normals[i + 1] + normals[i + 4] + normals[i + 7]) / 3;
		float nz = (normals[i + 2] + normals[i + 5] + normals[i + 8]) / 3;

		glVertex3f(vx, vy, vz);

		glVertex3f(vx + (normals[i] * 0.5f),
			vy + (normals[i + 1] * 0.5f),
			vz + (normals[i + 2]) * 0.5f);
	}

	glColor3f(1.0f, 1.0f, 1.0f);

	glEnd();
}

void Mesh::PrimitiveMesh(par_shapes_mesh* primitiveMesh)
{
	//vertexNum = primitiveMesh->npoints;
	//indexNum = primitiveMesh->ntriangles * 3;
	//normalNum = primitiveMesh->ntriangles;

	vertices = (float*)malloc(primitiveMesh->npoints);
	normals = (float*)malloc(primitiveMesh->ntriangles);
	indices = (uint*)malloc(primitiveMesh->ntriangles * 3);
	texCoords = (float*)malloc(primitiveMesh->npoints);

	verticesSizeBytes = sizeof(float) * primitiveMesh->npoints;
	normalsSizeBytes = sizeof(float) * primitiveMesh->ntriangles;
	texCoordSizeBytes = sizeof(uint) * primitiveMesh->ntriangles * 3;
	indicesSizeBytes = sizeof(float) * primitiveMesh->npoints;

	par_shapes_compute_normals(primitiveMesh);
	for (size_t i = 0; i < primitiveMesh->npoints; ++i)
	{
		memcpy(&vertices[i], &primitiveMesh->points[i * 3], sizeof(float) * 3);
		memcpy(&normals[i], &primitiveMesh->normals[i * 3], sizeof(float) * 3);
		if (primitiveMesh->tcoords != nullptr)
		{
			memcpy(&texCoords[i], &primitiveMesh->tcoords[i * 2], sizeof(float) * 2);
		}
		else if (meshType == Shape::CUBE)
		{
			switch (i % 4)
			{
			case 0:
				texCoords[i] = 0.0f;
				texCoords[i + 1] = 0.0f;
				break;
			case 1:
				texCoords[i] = 1.0f;
				texCoords[i + 1] = 0.0f;
				break;
			case 2:
				texCoords[i] = 1.0f;
				texCoords[i + 1] = 1.0f;
				break;
			case 3:
				texCoords[i] = 0.0f;
				texCoords[i + 1] = 1.0f;
				break;
			}
		}
		else
		{
			texCoords[i] = 0.0f;
			texCoords[i + 1] = 0.0f;
		}
	}

	for (size_t i = 0; i < (primitiveMesh->ntriangles * 3); ++i)
	{
		indices[i] = primitiveMesh->triangles[i];
	}

	memcpy(&normals[0], primitiveMesh->normals, primitiveMesh->npoints);

	par_shapes_free_mesh(primitiveMesh);
}

void Mesh::GetBoneTransforms(float timeInSeconds, std::vector<float4x4>& transforms, GameObject* gameObject)
{
	transforms.resize(boneInfo.size());

	float4x4 identity = float4x4::identity;

	float ticksPerSecond = (float)(assimpScene->mAnimations[0]->mTicksPerSecond != 0 ? assimpScene->mAnimations[0]->mTicksPerSecond : 25.0f);
	float timeInTicks = timeInSeconds * ticksPerSecond;

	float startFrame, endFrame, animDur;
	ComponentAnimator* cAnimator = gameObject->GetComponent<ComponentAnimator>();
	if (cAnimator != nullptr && cAnimator->GetSelectedClip() != nullptr)
	{
		startFrame = cAnimator->GetSelectedClip()->GetStartFrame();
		endFrame = cAnimator->GetSelectedClip()->GetEndFrame();
		animDur = endFrame - startFrame;
	}
	else
	{
		startFrame = 0.0f;
		endFrame = 0.0f;
		float animDur = assimpScene->mAnimations[0]->mDuration;
	}
	

	float animationTimeTicks = fmod(timeInTicks, (float)animDur); // This divides the whole animation into segments of animDur.

	ReadNodeHeirarchy(animationTimeTicks + startFrame, assimpScene->mRootNode, identity); // We add startFrame as an offset to the duration.
	transforms.resize(boneInfo.size());

	for (uint i = 0; i < boneInfo.size(); i++)
	{
		transforms[i] = boneInfo[i].finalTransformation;
	}
}

void Mesh::ReadNodeHeirarchy(float animationTimeTicks, const aiNode* pNode, const float4x4& parentTransform)
{
	std::string nodeName(pNode->mName.data);

	const aiAnimation* pAnimation = assimpScene->mAnimations[0];

	aiMatrix4x4 transformation = pNode->mTransformation;
	float4x4 nodeTransformation(Importer::GetInstance()->meshImporter->aiMatrix2Float4x4(transformation));

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, nodeName);

	if (pNodeAnim)
	{
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D scaling;
		CalcInterpolatedScaling(scaling, animationTimeTicks, pNodeAnim);
		float4x4 scalingM = InitScaleTransform(scaling.x, scaling.y, scaling.z);

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion rotationQ;
		CalcInterpolatedRotation(rotationQ, animationTimeTicks, pNodeAnim);
		float4x4 rotationM = aiMatrix3x32Float4x4(rotationQ.GetMatrix());

		// Interpolate translation and generate translation transformation matrix
		aiVector3D translation;
		CalcInterpolatedPosition(translation, animationTimeTicks, pNodeAnim);
		float4x4 translationM = InitTranslationTransform(translation.x, translation.y, translation.z);

		// Combine the above transformations
		nodeTransformation = translationM * rotationM * scalingM;
	}

	float4x4 globalTransformation = parentTransform * nodeTransformation;

	if (boneNameToIndexMap.find(nodeName) != boneNameToIndexMap.end())
	{
		uint boneIndex = boneNameToIndexMap[nodeName];
		float4x4 rootTransform = Importer::GetInstance()->meshImporter->aiMatrix2Float4x4(assimpScene->mRootNode->mTransformation.Inverse());
		float4x4 delta = rootTransform * globalTransformation * boneInfo[boneIndex].offsetMatrix;
		boneInfo[boneIndex].finalTransformation = delta.Transposed();
	}

	for (uint i = 0; i < pNode->mNumChildren; i++)
	{
		ReadNodeHeirarchy(animationTimeTicks, pNode->mChildren[i], globalTransformation);
	}
}

void Mesh::SetRootNode(const aiScene* assimpScene)
{
	this->assimpScene = assimpScene;
}

const aiNodeAnim* Mesh::FindNodeAnim(const aiAnimation* pAnimation, const std::string nodeName)
{
	for (uint i = 0; i < pAnimation->mNumChannels; i++)
	{
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (std::string(pNodeAnim->mNodeName.data) == nodeName)
		{
			return pNodeAnim;
		}
	}

	return nullptr;
}

uint Mesh::FindPosition(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
{
	for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		float t = (float)pNodeAnim->mPositionKeys[i + 1].mTime;
		if (AnimationTimeTicks < t) {
			return i;
		}
	}

	return 0;
}

void Mesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	uint PositionIndex = FindPosition(AnimationTimeTicks, pNodeAnim);
	uint NextPositionIndex = PositionIndex + 1;
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float t1 = (float)pNodeAnim->mPositionKeys[PositionIndex].mTime;
	float t2 = (float)pNodeAnim->mPositionKeys[NextPositionIndex].mTime;
	float DeltaTime = t2 - t1;
	float Factor = (AnimationTimeTicks - t1) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

uint Mesh::FindRotation(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		float t = (float)pNodeAnim->mRotationKeys[i + 1].mTime;
		if (AnimationTimeTicks < t) {
			return i;
		}
	}

	return 0;
}


void Mesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	uint RotationIndex = FindRotation(AnimationTimeTicks, pNodeAnim);
	uint NextRotationIndex = RotationIndex + 1;
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float t1 = (float)pNodeAnim->mRotationKeys[RotationIndex].mTime;
	float t2 = (float)pNodeAnim->mRotationKeys[NextRotationIndex].mTime;
	float DeltaTime = t2 - t1;
	float Factor = (AnimationTimeTicks - t1) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = StartRotationQ;
	Out.Normalize();
}


uint Mesh::FindScaling(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		float t = (float)pNodeAnim->mScalingKeys[i + 1].mTime;
		if (AnimationTimeTicks < t) {
			return i;
		}
	}

	return 0;
}

void Mesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	uint ScalingIndex = FindScaling(AnimationTimeTicks, pNodeAnim);
	uint NextScalingIndex = ScalingIndex + 1;
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float t1 = (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime;
	float t2 = (float)pNodeAnim->mScalingKeys[NextScalingIndex].mTime;
	float DeltaTime = t2 - t1;
	float Factor = (AnimationTimeTicks - (float)t1) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

float4x4 Mesh::InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ)
{
	float4x4 m;

	m[0][0] = ScaleX; m[0][1] = 0.0f;   m[0][2] = 0.0f;   m[0][3] = 0.0f;
	m[1][0] = 0.0f;   m[1][1] = ScaleY; m[1][2] = 0.0f;   m[1][3] = 0.0f;
	m[2][0] = 0.0f;   m[2][1] = 0.0f;   m[2][2] = ScaleZ; m[2][3] = 0.0f;
	m[3][0] = 0.0f;   m[3][1] = 0.0f;   m[3][2] = 0.0f;   m[3][3] = 1.0f;

	return m;
}

float4x4 Mesh::InitRotateTransform(const aiQuaternion& quat)
{
	float4x4 m;

	float yy2 = 2.0f * quat.y * quat.y;
	float xy2 = 2.0f * quat.x * quat.y;
	float xz2 = 2.0f * quat.x * quat.z;
	float yz2 = 2.0f * quat.y * quat.z;
	float zz2 = 2.0f * quat.z * quat.z;
	float wz2 = 2.0f * quat.w * quat.z;
	float wy2 = 2.0f * quat.w * quat.y;
	float wx2 = 2.0f * quat.w * quat.x;
	float xx2 = 2.0f * quat.x * quat.x;
	m[0][0] = -yy2 - zz2 + 1.0f;
	m[0][1] = xy2 + wz2;
	m[0][2] = xz2 - wy2;
	m[0][3] = 0;
	m[1][0] = xy2 - wz2;
	m[1][1] = -xx2 - zz2 + 1.0f;
	m[1][2] = yz2 + wx2;
	m[1][3] = 0;
	m[2][0] = xz2 + wy2;
	m[2][1] = yz2 - wx2;
	m[2][2] = -xx2 - yy2 + 1.0f;
	m[2][3] = 0.0f;
	m[3][0] = m[3][1] = m[3][2] = 0;
	m[3][3] = 1.0f;

	return m;
}

float4x4 Mesh::InitTranslationTransform(float x, float y, float z)
{
	float4x4 m;

	m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
	m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
	m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
	m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;

	return m;
}

float4x4 Mesh::aiMatrix3x32Float4x4(aiMatrix3x3 assimpMatrix)
{
	float4x4 m;

	m[0][0] = assimpMatrix.a1; m[0][1] = assimpMatrix.a2; m[0][2] = assimpMatrix.a3; m[0][3] = 0.0f;
	m[1][0] = assimpMatrix.b1; m[1][1] = assimpMatrix.b2; m[1][2] = assimpMatrix.b3; m[1][3] = 0.0f;
	m[2][0] = assimpMatrix.c1; m[2][1] = assimpMatrix.c2; m[2][2] = assimpMatrix.c3; m[2][3] = 0.0f;
	m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;

	return m;
}