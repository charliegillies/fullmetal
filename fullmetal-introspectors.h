#pragma once

#include "fullmetal-config.h"

#ifdef FM_EDITOR

namespace fm
{
	class SceneNode;
	class ShapeNode;
	class LightNode;

	class CubeNode;
	class SphereNode;
	class PlaneNode;
	class AmbientLightNode;
	class DirectionalLightNode;
	class SpotLightNode;
	class MeshNode;

	class Transform;
	class Vector3;
	struct Color;

	namespace gui
	{
		void introspectSceneNode(SceneNode* sceneNode);

		void introspectShapeNode(ShapeNode* sceneNode);

		void introspectLightNode(LightNode* lightNode);

		void introspectCubeNode(CubeNode* cubeNode);

		void introspectSphereNode(SphereNode* sphereNode);

		void introspectPlaneNode(PlaneNode* planeNode);

		void introspectAmbientLightNode(AmbientLightNode* ambientNode);

		void introspectDirectionalLightNode(DirectionalLightNode* lightNode);

		void introspectSpotLightNode(SpotLightNode* spotLight);

		void introspectMeshNode(MeshNode* meshNode);
	}
}

#endif