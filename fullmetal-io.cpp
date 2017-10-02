#include "fullmetal-io.h"

#ifdef FM_IO

#include "fullmetal.h"
#include "fullmetal-types.h"
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

void fm::io::transform_to_json(json & json, Transform & transform)
{
	vector3_to_json(json["position"], transform.position);
	vector3_to_json(json["scale"], transform.scale);
	vector3_to_json(json["rotation"], transform.rotation);
	json["angle"] = transform.angle;
}

void fm::io::transform_from_json(json & json, Transform & transform)
{
	auto& pos = json["position"];
	auto& scale = json["scale"];
	auto& rot = json["rotation"];
	auto& angle = json["angle"];

	vector3_from_json(pos, transform.position);
	vector3_from_json(scale, transform.scale);
	vector3_from_json(rot, transform.rotation);
	transform.angle = angle.get<float>();
}

void fm::io::vector3_to_json(json & json, Vector3 & vec)
{
	json["x"] = (float)vec.x;
	json["y"] = (float)vec.y;
	json["z"] = (float)vec.z;
}

void fm::io::vector3_from_json(json & json, Vector3 & vec)
{
	vec.x = json.at("x").get<float>();
	vec.y = json.at("y").get<float>();
	vec.z = json.at("z").get<float>();
}

void fm::io::color_to_json(json & json, Color & color)
{
	json["r"] = (float)color.r;
	json["g"] = (float)color.g;
	json["b"] = (float)color.b;
	json["a"] = (float)color.a;
}

void fm::io::color_from_json(json & json, Color & color)
{
	color.r = json.at("r").get<float>();
	color.g = json.at("g").get<float>();
	color.b = json.at("b").get<float>();
	color.a = json.at("a").get<float>();
}

// SCENE NODE TO JSON
void fm::io::SceneNode_to_json(json & j, SceneNode & node)
{
	json jTransform;
	transform_to_json(jTransform, node.transform);
	j["transform"] = jTransform;

	j["name"] = node.name;
	j["enabled"] = node.enabled;
}

void fm::io::SceneNode_from_json(json & j, SceneNode & node)
{
	json& jTransform = j["transform"];
	transform_from_json(jTransform, node.transform);

	json& jName = j["name"];
	json& jEnabled = j["enabled"];

	if (!jName.is_null())
		node.name = jName.get<std::string>();
	if (!jEnabled.is_null())
		node.enabled = jEnabled;
}

// SHAPE NODE TO JSON
void fm::io::ShapeNode_to_json(json & j, ShapeNode & shape)
{
	json jColor;
	color_to_json(jColor, shape.color);
	j["color"] = jColor;
}

void fm::io::ShapeNode_from_json(json & j, ShapeNode & shape)
{
	json& jColor = j["color"];
	color_from_json(jColor, shape.color);
}

// CUBE NODE TO JSON
void fm::io::CubeNode_to_json(json & j, CubeNode & cube)
{
	SceneNode_to_json(j, cube);
	ShapeNode_to_json(j, cube);
}

void fm::io::CubeNode_from_json(json & j, CubeNode & cube)
{
	SceneNode_from_json(j, cube);
	ShapeNode_from_json(j, cube);
}

// SPHERE NODE TO JSON
void fm::io::SphereNode_to_json(json & j, SphereNode & sphere)
{
	SceneNode_to_json(j, sphere);
	ShapeNode_to_json(j, sphere);
}

void fm::io::SphereNode_from_json(json & j, SphereNode & sphere)
{
	SceneNode_from_json(j, sphere);
	ShapeNode_from_json(j, sphere);
}

// PLANE NODE TO JSON
void fm::io::PlaneNode_to_json(json & j, PlaneNode & plane)
{
	SceneNode_to_json(j, plane);
	ShapeNode_to_json(j, plane);

	j["width"] = plane.width();
	j["height"] = plane.height();
	j["quadSize"] = plane.quadLength();
}

void fm::io::PlaneNode_from_json(json & j, PlaneNode & plane)
{
	SceneNode_from_json(j, plane);
	ShapeNode_from_json(j, plane);
	plane.buildQuads(j["quadSize"], j["width"], j["height"]);
}

// LIGHT NODE TO JSON
void fm::io::LightNode_to_json(json & j, LightNode & light)
{
	json jColor;
	color_to_json(jColor, light.color);
	j["color"] = jColor;
}

void fm::io::LightNode_from_json(json & j, LightNode & light)
{
	json& jColor = j["color"];
	color_from_json(jColor, light.color);
}

// AMBIENT LIGHT NODE TO JSON
void fm::io::AmbientLightNode_to_json(json & j, AmbientLightNode & ambientLight)
{
	SceneNode_to_json(j, ambientLight);
	LightNode_to_json(j, ambientLight);

	json jDiffuse;
	color_to_json(jDiffuse, ambientLight.diffuse);
	j["diffuse"] = jDiffuse;
}

void fm::io::AmbientLightNode_from_json(json & j, AmbientLightNode & ambientLight)
{
	SceneNode_from_json(j, ambientLight);
	LightNode_from_json(j, ambientLight);

	color_from_json(j["diffuse"], ambientLight.diffuse);
}

// DIRECTIONAL LIGHT TO JSON
void fm::io::DirectionalLightNode_to_json(json & j, DirectionalLightNode & directionalLight)
{
	SceneNode_to_json(j, directionalLight);
	LightNode_to_json(j, directionalLight);
}

void fm::io::DirectionalLightNode_from_json(json & j, DirectionalLightNode & directionalLight)
{
	SceneNode_from_json(j, directionalLight);
	LightNode_from_json(j, directionalLight);
}

// SPOTLIGHT TO JSON
void fm::io::SpotLight_to_json(json & j, SpotLightNode & spotLight)
{
	SceneNode_to_json(j, spotLight);
	LightNode_to_json(j, spotLight);

	j["cutoff"] = spotLight.cutoff;
	j["exponent"] = spotLight.exponent;

	json jDirection;
	vector3_to_json(jDirection, spotLight.direction);
	j["direction"] = jDirection;

	json jDiffuse;
	color_to_json(jDiffuse, spotLight.diffuse);
	j["diffuse"] = jDiffuse;
}

void fm::io::Spotlight_from_json(json & j, SpotLightNode & spotLight)
{
	SceneNode_from_json(j, spotLight);
	LightNode_from_json(j, spotLight);

	spotLight.cutoff = j["cutoff"];
	spotLight.exponent = j["exponent"];

	vector3_to_json(j["direction"], spotLight.direction);
	color_to_json(j["diffuse"], spotLight.diffuse);
}

#endif // END