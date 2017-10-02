/*
 * Contains the implementations of the SceneNode.introspect() methods.
 */

#include "fullmetal-config.h"
#ifdef FM_EDITOR

// include fullmetal, gui & imgui
#include "fullmetal.h"
#include "fullmetal-gui.h"
#include "imgui/imgui.h"

void fm::SceneNode::introspect() {
	fm::gui::introspectTransform(transform);
}

void fm::ShapeNode::introspect() {
	SceneNode::introspect();
	fm::gui::introspectColor(color);
}

void fm::CubeNode::introspect() {
	ShapeNode::introspect();
}

void fm::SphereNode::introspect() {
	ShapeNode::introspect();
}

void fm::PlaneNode::introspect() {
	ShapeNode::introspect();

	ImGui::Text("Plane Settings");
	ImGui::Indent();

	int w = _width;
	int h = _height;
	int qSize = _quadSize;
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

	// show diagnostics..
	ImGui::LabelText("Num Tris", std::to_string(_tris.size()).c_str());

	// check if we need to rebuild our quads
	if (rebuild)
		buildQuads(qSize, w, h);

	ImGui::Unindent();
}

void fm::LightNode::introspect() {
	SceneNode::introspect();
	fm::gui::introspectColor(color);
}

void fm::AmbientLightNode::introspect() {
	LightNode::introspect();
	fm::gui::introspectColor(diffuse, "Diffuse Color");
}

void fm::DirectionalLightNode::introspect() {
	LightNode::introspect();
}

void fm::SpotLightNode::introspect() {
	LightNode::introspect();

	fm::gui::introspectColor(diffuse, "Diffuse Color");

	// begin spot light properties..
	ImGui::Text("Spot Light Settings");
	ImGui::Indent();

	fm::gui::introspectVector3(direction, "direction");
	ImGui::InputFloat("Cutoff", &cutoff);
	ImGui::InputFloat("Exponent", &exponent);

	ImGui::Unindent();
}

#endif