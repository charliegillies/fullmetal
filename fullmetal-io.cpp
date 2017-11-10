#include "fullmetal-io.h"

#ifdef FM_IO

#include "fullmetal.h"
#include "fullmetal-types.h"
#include "fullmetal-3d.h"
#include "json.hpp"

#include <fstream>

void fm::io::writeJson(std::string & file, nlohmann::json & j)
{
	// write the json with 4 spaces/tab indentation for neat reading
	std::ofstream stream(file.c_str());
	stream << j.dump(4) << std::endl;
	stream.close();
}

nlohmann::json fm::io::readJson(std::string & file)
{
	std::ifstream stream;
	stream.open(file.c_str());
	assert(stream.good());

	std::string data, line;
	while (std::getline(stream, line))
		data += (line + "\n");
	
	stream.close();
	nlohmann::json json = json::parse(data.c_str());
	return json;
}

fm::SceneNodeGraph* fm::io::readSceneGraph(std::string file, NodeTypeTable* typeTable)
{
	// create a scene graph
	SceneNodeGraph* graph = new SceneNodeGraph();

	// read the json file, get the nodes array
	nlohmann::json json = readJson(file);
	nlohmann::json jNodes = json["nodes"];

	// parse each node, add it to the scene graph
	for (auto i = jNodes.begin(); i != jNodes.end(); ++i) {
		auto jNode = *i;
		//TODO null check against readNode()
		graph->addNode(readNode(jNode, typeTable));
	}

	return graph;
}

void fm::io::writeSceneGraph(std::string file, SceneNodeGraph* sceneGraph, NodeTypeTable* typeTable)
{
	// base object, our graph
	nlohmann::json jGraph = nlohmann::json::object();
	// our array of nodes that we are about to write
	nlohmann::json jNodes = nlohmann::json::array();

	for (auto node : sceneGraph->getNodes()) {
		// our node json object
		nlohmann::json jNode = nlohmann::json::object();
		// write the node into json
		writeNode(jNode, node, typeTable);
		// push back into the array
		jNodes.push_back(jNode);
	}

	jGraph["nodes"] = jNodes;
	writeJson(file, jGraph);
}

void fm::io::writeNode(nlohmann::json& json, SceneNode* node, NodeTypeTable* typeTable)
{
	// use the typetable to write our node
	typeTable->writeNode(json, node);

	// check for any children of our node
	std::vector<SceneNode*>& children = node->childNodes;

	// if we have children..
	if (children.size() > 0) {
		// create a json array "children"
		auto jChildren = nlohmann::json::array();

		// then fill that array with child nodes that
		// we write by recursively calling our writeNode method.
		for (auto child : children) {
			nlohmann::json jChild = nlohmann::json::object();
			writeNode(jChild, child, typeTable);
			jChildren.push_back(jChild);
		}

		json["children"] = jChildren;
	}
}

fm::SceneNode* fm::io::readNode(nlohmann::json & json, NodeTypeTable * typeTable)
{
	SceneNode* node = typeTable->readNode(json);
	// try to get the json 'children' field, which is an array of child nodes
	nlohmann::json jChildren = json["children"];
	// if not null, attempt to parse them
	if (!jChildren.is_null()) {
		// if we parse a child, add it as a node child so the tree stays in shape
		for (auto& jChild : jChildren) {
			auto nChild = readNode(jChild, typeTable);
			//TODO null check
			node->addChild(nChild);
		}
	}

	return node;
}

void fm::io::writeTransform(json & json, Transform & transform)
{
	writeVector3(json["position"], transform.position);
	writeVector3(json["scale"], transform.scale);
	writeVector3(json["rotation"], transform.rotation);
	json["angle"] = transform.angle;
}

void fm::io::readTransform(json & json, Transform & transform)
{
	auto& pos = json["position"];
	auto& scale = json["scale"];
	auto& rot = json["rotation"];
	auto& angle = json["angle"];

	readVector3(pos, transform.position);
	readVector3(scale, transform.scale);
	readVector3(rot, transform.rotation);
	transform.angle = angle.get<float>();
}

void fm::io::writeVector3(json & json, Vector3 & vec)
{
	json["x"] = (float)vec.x;
	json["y"] = (float)vec.y;
	json["z"] = (float)vec.z;
}

void fm::io::readVector3(json & json, Vector3 & vec)
{
	vec.x = json.at("x").get<float>();
	vec.y = json.at("y").get<float>();
	vec.z = json.at("z").get<float>();
}

void fm::io::writeColor(json & json, Color & color)
{
	json["r"] = (float)color.r;
	json["g"] = (float)color.g;
	json["b"] = (float)color.b;
	json["a"] = (float)color.a;
}

void fm::io::readColor(json & json, Color & color)
{
	color.r = json.at("r").get<float>();
	color.g = json.at("g").get<float>();
	color.b = json.at("b").get<float>();
	color.a = json.at("a").get<float>();
}

void fm::io::writeMaterial(json & j, Material & material)
{
	json ambColorJson = json::object();
	json difColorJson = json::object();
	json specColorJson;
	json shininessJson;

	writeColor(ambColorJson, material.ambientColor);
	writeColor(difColorJson, material.diffuseColor);

	if (material.specularEnabled) {
		specColorJson = json::object();
		writeColor(specColorJson, material.specularColor);
	}

	if (material.shininessEnabled) {
		shininessJson = json::object();
		shininessJson = material.shininess;
	}

	j["ambColor"] = ambColorJson;
	j["difColor"] = difColorJson;
	j["specColor"] = specColorJson; // could be null
	j["shininess"] = shininessJson; // could be null
}

void fm::io::readMaterial(json & j, Material & material)
{
	json ambColorJson = j["ambColor"];
	json difColorJson = j["difColor"];
	json specColorJson = j["specColor"];
	json shininessJson = j["shininess"];

	readColor(ambColorJson, material.ambientColor);
	readColor(difColorJson, material.diffuseColor);

	if (!specColorJson.is_null()) {
		material.specularEnabled = true;
		readColor(specColorJson, material.specularColor);
	}

	if (!shininessJson.is_null()) {
		material.shininessEnabled = true;
		material.shininess = shininessJson;
	}
}

// SCENE NODE TO JSON
void fm::io::writeSceneNode(json & j, SceneNode & node)
{
	json jTransform;
	writeTransform(jTransform, node.transform);
	j["transform"] = jTransform;

	j["name"] = node.name;
	j["enabled"] = node.enabled;
}

void fm::io::readSceneNode(json & j, SceneNode & node)
{
	json& jTransform = j["transform"];
	readTransform(jTransform, node.transform);

	json& jName = j["name"];
	json& jEnabled = j["enabled"];

	if (!jName.is_null())
		node.name = jName.get<std::string>();
	if (!jEnabled.is_null())
		node.enabled = jEnabled;
}

// SHAPE NODE TO JSON
void fm::io::writeShapeNode(json & j, ShapeNode & shape)
{
	json jMaterial = json::object();
	writeMaterial(jMaterial, shape.material);
	j["material"] = jMaterial;
}

void fm::io::readShapeNode(json & j, ShapeNode & shape)
{
	readMaterial(j["material"], shape.material);
}

// CUBE NODE TO JSON
void fm::io::writeCubeNode(json & j, CubeNode & cube)
{
	writeSceneNode(j, cube);
	writeShapeNode(j, cube);
}

void fm::io::readCubeNode(json & j, CubeNode & cube)
{
	readSceneNode(j, cube);
	readShapeNode(j, cube);
}

// SPHERE NODE TO JSON
void fm::io::writeSphereNode(json & j, SphereNode & sphere)
{
	writeSceneNode(j, sphere);
	writeShapeNode(j, sphere);

	j["stacks"] = sphere.getStacks();
	j["slices"] = sphere.getSlices();
}

void fm::io::readSphereNode(json & j, SphereNode & sphere)
{
	readSceneNode(j, sphere);
	readShapeNode(j, sphere);

	int& stacks = sphere.getStacks();
	int& slices = sphere.getSlices();
	stacks = j["stacks"];
	slices = j["slices"];
}

// PLANE NODE TO JSON
void fm::io::writePlaneNode(json & j, PlaneNode & plane)
{
	writeSceneNode(j, plane);
	writeShapeNode(j, plane);

	j["width"] = plane.width();
	j["height"] = plane.height();
	j["quadSize"] = plane.quadLength();
}

void fm::io::readPlaneNode(json & j, PlaneNode & plane)
{
	readSceneNode(j, plane);
	readShapeNode(j, plane);
	plane.buildQuads(j["quadSize"], j["width"], j["height"]);
}

// LIGHT NODE TO JSON
void fm::io::writeLightNode(json & j, LightNode & light)
{
	json jColor;
	writeColor(jColor, light.color);
	j["color"] = jColor;
}

void fm::io::readLightNode(json & j, LightNode & light)
{
	json& jColor = j["color"];
	readColor(jColor, light.color);
}

// AMBIENT LIGHT NODE TO JSON
void fm::io::writeAmbientLightNode(json & j, AmbientLightNode & ambientLight)
{
	writeSceneNode(j, ambientLight);
	writeLightNode(j, ambientLight);

	json jDiffuse;
	writeColor(jDiffuse, ambientLight.diffuse);
	j["diffuse"] = jDiffuse;
}

void fm::io::readAmbientLightNode(json & j, AmbientLightNode & ambientLight)
{
	readSceneNode(j, ambientLight);
	readLightNode(j, ambientLight);

	readColor(j["diffuse"], ambientLight.diffuse);
}

// DIRECTIONAL LIGHT TO JSON
void fm::io::writeDirectionalLightNode(json & j, DirectionalLightNode & directionalLight)
{
	writeSceneNode(j, directionalLight);
	writeLightNode(j, directionalLight);
}

void fm::io::readDirectionalLightNode(json & j, DirectionalLightNode & directionalLight)
{
	readSceneNode(j, directionalLight);
	readLightNode(j, directionalLight);
}

// SPOTLIGHT TO JSON
void fm::io::writeSpotLightNode(json & j, SpotLightNode & spotLight)
{
	writeSceneNode(j, spotLight);
	writeLightNode(j, spotLight);

	j["cutoff"] = spotLight.cutoff;
	j["exponent"] = spotLight.exponent;

	json jDirection;
	writeVector3(jDirection, spotLight.direction);
	j["direction"] = jDirection;

	json jDiffuse;
	writeColor(jDiffuse, spotLight.diffuse);
	j["diffuse"] = jDiffuse;
}

void fm::io::readSpotLightNode(json & j, SpotLightNode & spotLight)
{
	readSceneNode(j, spotLight);
	readLightNode(j, spotLight);

	spotLight.cutoff = j["cutoff"];
	spotLight.exponent = j["exponent"];

	writeVector3(j["direction"], spotLight.direction);
	writeColor(j["diffuse"], spotLight.diffuse);
}

void fm::io::writeObjModel(json & j, ObjModel* model)
{
	j["filepath"] = model->filepath;
	j["switchedUvs"] = model->switchedUvs;
}

void fm::io::readObjModel(json & j, ObjModel** model)
{
	// get the filepath, load the obj model from it
	std::string filepath = j["filepath"];
	bool switched = j["switchedUvs"];

	ObjModel* m = AssetManager::global->getObjModel(filepath);
	*model = m;

	if (switched)
		switchModelUvs(m);
}

void fm::io::writeMeshNode(json & j, MeshNode & meshNode)
{
	writeSceneNode(j, meshNode);

	json jMaterial = json::object();
	writeMaterial(jMaterial, meshNode.material);
	j["material"] = jMaterial;

	json jModel;
	if (meshNode.model != nullptr) {
		jModel = json::object();
		writeObjModel(jModel, meshNode.model);
	}

	j["model"] = jModel;
}

void fm::io::readMeshNode(json & j, MeshNode & meshNode)
{
	readSceneNode(j, meshNode);

	readMaterial(j["material"], meshNode.material);

	json& jModel = j["model"];
	if (!jModel.is_null()) {
		readObjModel(jModel, &meshNode.model);
	}
}

void fm::io::writeCylinderNode(json & j, CylinderNode & node)
{
	writeSceneNode(j, node);
	writeShapeNode(j, node);

	j["segments"] = node.numSegments();
}

void fm::io::readCylinderNode(json & j, CylinderNode & node)
{
	readSceneNode(j, node);
	readShapeNode(j, node);

	node.build(j["segments"]);
}

#endif // END