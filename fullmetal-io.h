#pragma once
#include "fullmetal-config.h"

#ifdef FM_IO

#include <string>
#include "json.hpp"
using json = nlohmann::json;

namespace fm {
	class SceneNodeGraph;
	class NodeTypeTable;

	class SceneNode;
	class ShapeNode;
	class LightNode;
	class CubeNode;
	class SphereNode;
	class PlaneNode;
	class AmbientLightNode;
	class DirectionalLightNode;
	class SpotLightNode;

	class Transform;
	class Vector3;
	struct Color;

	namespace io { 
		/*
		 */
		void writeJson(std::string& file, nlohmann::json& j);

		/*
		 */
		nlohmann::json readJson(std::string& file);

		/*
		 * Reads the scene graph from the given file path.
		 */
		SceneNodeGraph* readSceneGraph(std::string file, NodeTypeTable* typeTable);

		/*
		 * Writes the scene graph to the given file path.
		 */
		void writeSceneGraph(std::string file, SceneNodeGraph* sceneGraph, NodeTypeTable* typeTable);

		/*
		 * Writes a node into json, not knowing the nodes child type.
		 */
		void writeNode(nlohmann::json& json, SceneNode* node, NodeTypeTable* typeTable);

		/* 
		 * 
		 */
		SceneNode* readNode(nlohmann::json& json, NodeTypeTable* typeTable);

		void transform_to_json(json& j, Transform& transform);
		void transform_from_json(json& j, Transform& transform);

		void vector3_to_json(json& j, Vector3& vec);
		void vector3_from_json(json& j, Vector3& vec);

		void color_to_json(json& j, Color& color);
		void color_from_json(json& j, Color& color);

		void SceneNode_to_json(json& j, SceneNode& node);
		void SceneNode_from_json(json& j, SceneNode& node);

		void ShapeNode_to_json(json& j, ShapeNode& shape);
		void ShapeNode_from_json(json& j, ShapeNode& shape);

		void CubeNode_to_json(json& j, CubeNode& cube);
		void CubeNode_from_json(json& j, CubeNode& cube);

		void SphereNode_to_json(json& j, SphereNode& sphere);
		void SphereNode_from_json(json& j, SphereNode& sphere);

		void PlaneNode_to_json(json& j, PlaneNode& plane);
		void PlaneNode_from_json(json& j, PlaneNode& plane);

		void LightNode_to_json(json& j, LightNode& light);
		void LightNode_from_json(json& j, LightNode& light);

		void AmbientLightNode_to_json(json& j, AmbientLightNode& ambientLight);
		void AmbientLightNode_from_json(json& j, AmbientLightNode& ambientLight);

		void DirectionalLightNode_to_json(json& j, DirectionalLightNode& directionalLight);
		void DirectionalLightNode_from_json(json& j, DirectionalLightNode& directionalLight);

		void SpotLight_to_json(json& j, SpotLightNode& spotLight);
		void Spotlight_from_json(json& j, SpotLightNode& spotLight);
	}
}

#endif