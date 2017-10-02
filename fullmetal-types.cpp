#include "fullmetal-types.h"

#include "fullmetal.h"
#include "fullmetal-io.h"

std::vector<std::string> fm::NodeTypeTable::getIds()
{
	std::vector<std::string> ids;
	for (auto link : _linksById)
		ids.push_back(link.first);

	return ids;
}

fm::SceneNode * fm::NodeTypeTable::createNodeFromId(std::string id)
{
	// get the link from the match, ensure it's not null
	// otherwise we have a dodgy id, which shouldn't ever
	// happen during the programs runtime
	auto link = _linksById[id];
	assert(link != nullptr);

	// create the node from the link
	return link->create_node();
}

fm::NodeTypeTable* fm::createDefaultTypeTable()
{
	fm::NodeTypeTable* nodeTable = new fm::NodeTypeTable();

	// register shape nodes..
	auto& cube = nodeTable->registerNode<fm::CubeNode>("CubeNode");
	auto& sphere = nodeTable->registerNode<fm::SphereNode>("SphereNode");
	auto& plane = nodeTable->registerNode<fm::PlaneNode>("PlaneNode");

	// register light nodes..
	auto& ambient_light = nodeTable->registerNode<fm::AmbientLightNode>("AmbientLightNode");
	auto& directional_light = nodeTable->registerNode<fm::DirectionalLightNode>("DirectionalLightNode");
	auto& spot_light = nodeTable->registerNode<fm::SpotLightNode>("SpotLightNode");
	
#ifdef FM_IO
	// if we're using IO, register the parse functions for reading/writing the nodes.
	cube.set_parse_functions(fm::io::CubeNode_from_json, fm::io::CubeNode_to_json);
	sphere.set_parse_functions(fm::io::SphereNode_from_json, fm::io::SphereNode_to_json);
	plane.set_parse_functions(fm::io::PlaneNode_from_json, fm::io::PlaneNode_to_json);

	ambient_light.set_parse_functions(fm::io::AmbientLightNode_from_json, 
		fm::io::AmbientLightNode_to_json);

	directional_light.set_parse_functions(fm::io::DirectionalLightNode_from_json, 
		fm::io::DirectionalLightNode_to_json);

	spot_light.set_parse_functions(fm::io::Spotlight_from_json, fm::io::SpotLight_to_json);
#endif

	return nodeTable;
}
