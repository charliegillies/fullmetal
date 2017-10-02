/*
 * Contains the implementations of the SceneNode.introspect() methods.
 */

#include "fullmetal-introspectors.h"

#ifdef FM_EDITOR

// include fullmetal, gui & imgui
#include "fullmetal.h"
#include "fullmetal-gui.h"
#include "imgui/imgui.h"

void fm::gui::introspectSceneNode(SceneNode * sceneNode)
{
	ImGui::Text("Node");
	ImGui::Indent();
	ImGui::Checkbox("Enabled", &sceneNode->enabled);
	fm::gui::guiString(sceneNode->name, "Name");
	ImGui::Unindent();

	introspectTransform(sceneNode->transform);
}

void fm::gui::introspectShapeNode(ShapeNode * sceneNode)
{
	introspectSceneNode(sceneNode);
	fm::gui::introspectColor(sceneNode->color);
}

void fm::gui::introspectLightNode(LightNode * lightNode)
{
	introspectSceneNode(lightNode);
	fm::gui::introspectColor(lightNode->color);
}

void fm::gui::introspectCubeNode(CubeNode * cubeNode)
{
	introspectShapeNode(cubeNode);
}

void fm::gui::introspectSphereNode(SphereNode * sphereNode)
{
	introspectShapeNode(sphereNode);
}

void fm::gui::introspectPlaneNode(PlaneNode * planeNode)
{
	introspectShapeNode(planeNode);

	ImGui::Text("Plane Settings");
	ImGui::Indent();

	int w = planeNode->width();
	int h = planeNode->height();
	int qSize = planeNode->quadLength();
	bool rebuild = false;

	// allow for resizing of the plane, amount of quads, etc.
	if (ImGui::InputInt("Quad Length", &qSize, 1)) {
		rebuild = true;

		if (qSize < 0.1f)
			qSize = 0.1f;
	}

	if (ImGui::InputInt("Width", &w, 1)) {
		rebuild = true;

		if (w < 1)
			w = 1;
	}

	if (ImGui::InputInt("Height", &h, 1)) {
		rebuild = true;

		if (h < 1)
			h = 1;
	}

	// check if we need to rebuild our quads
	if (rebuild)
		planeNode->buildQuads(qSize, w, h);

	ImGui::Unindent();
}

void fm::gui::introspectAmbientLightNode(AmbientLightNode * ambientNode)
{
	introspectLightNode(ambientNode);
	fm::gui::introspectColor(ambientNode->diffuse, "Diffuse Color");
}

void fm::gui::introspectDirectionalLightNode(DirectionalLightNode * lightNode)
{
	introspectLightNode(lightNode);
}

void fm::gui::introspectSpotLightNode(SpotLightNode * spotLight)
{
	introspectLightNode(spotLight);

	fm::gui::introspectColor(spotLight->diffuse, "Diffuse Color");

	// begin spot light properties..
	ImGui::Text("Spot Light Settings");
	ImGui::Indent();

	fm::gui::introspectVector3(spotLight->direction, "direction");
	ImGui::InputFloat("Cutoff", &spotLight->cutoff);
	ImGui::InputFloat("Exponent", &spotLight->exponent);

	ImGui::Unindent();
}

#endif