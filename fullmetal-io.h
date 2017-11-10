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
	class MeshNode;
	class CylinderNode;

	class Transform;
	class Vector3;
	struct Color;

	struct Material;
	struct ObjModel;

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

		void writeTransform(json& j, Transform& transform);
		void readTransform(json& j, Transform& transform);

		void writeVector3(json& j, Vector3& vec);
		void readVector3(json& j, Vector3& vec);

		void writeColor(json& j, Color& color);
		void readColor(json& j, Color& color);

		void writeMaterial(json& j, Material& material);
		void readMaterial(json& j, Material& material);

		void writeSceneNode(json& j, SceneNode& node);
		void readSceneNode(json& j, SceneNode& node);

		void writeShapeNode(json& j, ShapeNode& shape);
		void readShapeNode(json& j, ShapeNode& shape);

		void writeCubeNode(json& j, CubeNode& cube);
		void readCubeNode(json& j, CubeNode& cube);

		void writeSphereNode(json& j, SphereNode& sphere);
		void readSphereNode(json& j, SphereNode& sphere);

		void writePlaneNode(json& j, PlaneNode& plane);
		void readPlaneNode(json& j, PlaneNode& plane);

		void writeLightNode(json& j, LightNode& light);
		void readLightNode(json& j, LightNode& light);

		void writeAmbientLightNode(json& j, AmbientLightNode& ambientLight);
		void readAmbientLightNode(json& j, AmbientLightNode& ambientLight);

		void writeDirectionalLightNode(json& j, DirectionalLightNode& directionalLight);
		void readDirectionalLightNode(json& j, DirectionalLightNode& directionalLight);

		void writeSpotLightNode(json& j, SpotLightNode& spotLight);
		void readSpotLightNode(json& j, SpotLightNode& spotLight);

		void writeObjModel(json& j, ObjModel* model);
		void readObjModel(json& j, ObjModel** model);

		void writeMeshNode(json& j, MeshNode& meshNode);
		void readMeshNode(json& j, MeshNode& meshNode);

		void writeCylinderNode(json& j, CylinderNode& node);
		void readCylinderNode(json& j, CylinderNode& node);
	}
}

#endif