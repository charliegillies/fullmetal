#include "fullmetal-types.h"

#include "fullmetal.h"
#include "fullmetal-io.h"
#include "fullmetal-introspectors.h"

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

	// 3d model nodes
	auto& mesh_node = nodeTable->registerNode<fm::MeshNode>("MeshNode");

#ifdef FM_IO
	// if we're using IO, register the parse functions for reading/writing the nodes.
	cube.set_parse_functions(fm::io::readCubeNode, fm::io::writeCubeNode);
	sphere.set_parse_functions(fm::io::readSphereNode, fm::io::writeSphereNode);
	plane.set_parse_functions(fm::io::readPlaneNode, fm::io::writePlaneNode);

	ambient_light.set_parse_functions(fm::io::readAmbientLightNode, 
		fm::io::writeAmbientLightNode);

	directional_light.set_parse_functions(fm::io::readDirectionalLightNode, 
		fm::io::writeDirectionalLightNode);

	spot_light.set_parse_functions(fm::io::readSpotLightNode, fm::io::writeSpotLightNode);

	mesh_node.set_parse_functions(fm::io::readMeshNode, fm::io::writeMeshNode);
#endif

#ifdef FM_EDITOR
	// if we're using the GUI, register the introspection functions for reading/writing
	cube.set_introspection_function(fm::gui::introspectCubeNode);
	sphere.set_introspection_function(fm::gui::introspectSphereNode);
	plane.set_introspection_function(fm::gui::introspectPlaneNode);

	ambient_light.set_introspection_function(fm::gui::introspectAmbientLightNode);
	directional_light.set_introspection_function(fm::gui::introspectDirectionalLightNode);
	spot_light.set_introspection_function(fm::gui::introspectSpotLightNode);

	mesh_node.set_introspection_function(fm::gui::introspectMeshNode);
#endif

	return nodeTable;
}
