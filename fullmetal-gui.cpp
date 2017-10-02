#include "fullmetal-gui.h"

#ifdef FM_EDITOR

// Include ImGui here!
#include "imgui/imgui.h"
// and also fullmetal!
#include "fullmetal.h"
#include "fullmetal-types.h"

#ifdef FM_IO
#include "fullmetal-io.h"
#endif

#include "glut.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include <stdio.h>
#include <string>

GLuint textureHandle = NULL;

fm::gui::GraphRenderConfig::GraphRenderConfig() : window_toggled(true), selected_node(nullptr) { }

void fm::gui::drawNodeGraph(SceneNodeGraph* nodeGraph, GraphRenderConfig* config, NodeTypeTable* typeTable)
{
	// Render the scene graph window
	if (ImGui::Begin("Scene Graph##tree", &config->window_toggled)) {

#ifdef FM_IO
		// Allow writing of the scene graph
		if (ImGui::Button("Save##tree")) {
			fm::io::writeSceneGraph(config->filepath, nodeGraph, typeTable);
		}

		ImGui::SameLine();

		// Show the filepath that we're using..
		ImGui::Text(config->filepath.c_str());
		ImGui::Separator();
#endif

		// Displays the number of nodes inside the scene
		ImGui::LabelText("Scene Node Count", std::to_string(nodeGraph->nodeCount()).c_str());

		// if we're provided with a type table, show the add option
		if (typeTable != nullptr) {
			drawAddNodeOptions(nodeGraph, config, typeTable);
		}

		// Begins the child-tree element of the parent window
		if (ImGui::BeginChild("Nodes##tree", ImVec2{ 0, 250 }, true)) {
			// render the node tree
			drawNodes(nodeGraph->getNodes(), config);
			ImGui::EndChild();
		}

		// Show the clicked node, if any
		if (config->selected_node != nullptr) {
			if (ImGui::BeginChild("Selected Node##tree", ImVec2{ }, true)) {
				typeTable->introspect(config->selected_node);
				ImGui::EndChild();
			}
		}

		ImGui::End();
	}
}

void fm::gui::drawAddNodeOptions(SceneNodeGraph* nodeGraph, GraphRenderConfig* graphConfig, NodeTypeTable* typeTable)
{
	static int nodeIndex = 0;

	// draw all node ids
	auto node_ids = typeTable->getIds();
	drawComboBox("Nodes", node_ids, nodeIndex);
	std::string& id = node_ids[nodeIndex];

	// draw an 'create' button, this one adds the node to the scene
	if (ImGui::Button("Create scene node")) {
		nodeGraph->addNode(typeTable->createNodeFromId(id));
	}

	// if we have a selected node already, allow child node creation,
	// deleting of nodes, moving of nodes own hierarchy
	if (graphConfig->selected_node != nullptr) {

		// CREATE CHILD NODE
		ImGui::SameLine();
		if (ImGui::Button("Create child node")) {
			graphConfig->selected_node->addChild(typeTable->createNodeFromId(id));
		}

		// DELETE NODE
		ImGui::SameLine();
		if (ImGui::Button("Delete node")) {
			deleteNodeFromGraph(graphConfig, nodeGraph);
		}

		//TODO implement hiearchy traversal
		//// DOWN IN HIERARCHY
		//ImGui::SameLine();
		//if (ImGui::Button("Up")) {

		//}

		//// UP IN HIERARCHY
		//ImGui::SameLine();
		//if (ImGui::Button("Down")) {

		//}
	}
}

void fm::gui::deleteNodeFromGraph(fm::gui::GraphRenderConfig * graphConfig, fm::SceneNodeGraph * nodeGraph)
{
	// get the parent of the selected node..
	auto node_parent = graphConfig->selected_node->getParent();

	// if no parent, it's a top level node
	// so we need to remove it from the graph itself
	if (node_parent == nullptr) {
		// This removes the node but does not remove it from memory
		nodeGraph->removeNode(graphConfig->selected_node);
		delete graphConfig->selected_node;
		graphConfig->selected_node = nullptr;
	}
	else {
		// this means that we have a child node, so we need to remove it
		// from the parent, rather than the graph
		auto child = node_parent->removeChild(graphConfig->selected_node);
		delete child;
		graphConfig->selected_node = nullptr;
	}
}

void fm::gui::drawComboBox(std::string title, std::vector<std::string>& items, int & comboIndex)
{
	// this is a lamda, a method we pass in to imgui::combo that fetches
	// the text value that we want to display in our combo box
	// by giving us a ptr to our data, the index of that data and a ptr to the char*
	auto getItem = [](void* data, int idx, const char** out_text) {
		// we get our vector back from our void* data ptr
		std::vector<std::string>* vec = reinterpret_cast<std::vector<std::string>*>(data);
		// ensure index is within bounds before we call .at(idx)
		if (idx < 0 || idx >= vec->size()) return false;

		// set our char ptr to our string
		*out_text = vec->at(idx).c_str();
		return true;
	};

	// then we just call combo and the lamda does all the work
	ImGui::Combo(title.c_str(), &comboIndex, getItem, reinterpret_cast<void*>(&items), items.size());
}

void fm::gui::drawNodes(std::vector<SceneNode*>& nodes, GraphRenderConfig* config)
{
	// Render every node in a tree order..
	for (int i = 0; i < nodes.size(); ++i) {
		SceneNode* node = nodes[i];
		
		// test if anything was clicked
		auto clicked = drawNodeSelect(node, config);

		// assign the node that was clicked, if any
		if (clicked != nullptr)
			config->selected_node = clicked;
	}
}

void fm::gui::guiString(std::string & str, std::string label, int bufSize)
{
	std::vector<char> buf{ str.begin(), str.end() };
	buf.resize(bufSize);

	ImGui::InputText(label.c_str(), buf.data(), bufSize);

	// set string to our buf data
	str = buf.data();
}

fm::SceneNode* fm::gui::drawNodeSelect(SceneNode * node, GraphRenderConfig* config)
{
	int childNodeCount = node->childNodes.size();

	std::string& name = node->name;
	std::string id = name + "##scenenode" + std::to_string(node->getUniqueId());
	
	bool nodeSelected = node == config->selected_node;
	SceneNode* clicked = nullptr;

	// if we have children we can open the node, otherwise it's a leaf.
	ImGuiTreeNodeFlags node_flags = (childNodeCount > 0) ?
		ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick
		: ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	// if our node is selected, appear selected.
	if (nodeSelected)
		node_flags = node_flags | ImGuiTreeNodeFlags_Selected;

	// draw the tree node, test if it's open..
	bool nodeOpen = ImGui::TreeNodeEx(id.c_str(), node_flags);
	// check if our tree node was selected
	if (ImGui::IsItemClicked() && clicked == nullptr)
		clicked = node;

	if (nodeOpen && childNodeCount > 0) {
		// render all children, poll for a node selection
		// if we select a node, test we haven't found a clicked one yet already
		for (auto child : node->childNodes) {
			auto n = drawNodeSelect(child, config);
			if (n != nullptr && clicked == nullptr)
				clicked = n;
		}
	}

	// Pop the tree node
	if(nodeOpen && childNodeCount > 0)
		ImGui::TreePop();

	return clicked;
}

void fm::gui::introspectColor(Color & color, std::string title)
{
	ImGui::Text(title.c_str());
	ImGui::Indent();

	ImGui::DragFloat("R", &color.r, 0.0025f, 0.0f, 1.0f);
	ImGui::DragFloat("G", &color.g, 0.0025f, 0.0f, 1.0f);
	ImGui::DragFloat("B", &color.b, 0.0025f, 0.0f, 1.0f);
	ImGui::DragFloat("A", &color.a, 0.0025f, 0.0f, 1.0f);

	//// translate opengl color (0-1.0f) to imColor (0-255)
	//int r = (int)(color.r*255.0f);
	//int g = (int)(color.g*255.0f);
	//int b = (int)(color.b*255.0f);
	//int a = (int)(color.a*255.0f);

	//ImColor imcolor = ImColor(r, g, b, a);

	//if (ImGui::ColorPicker3("Color", (float*)&imcolor)) {
	//	color.r = imcolor.Value.x / 255.0f;
	//	color.g = imcolor.Value.y / 255.0f;
	//	color.b = imcolor.Value.z / 255.0f;
	//	color.a = imcolor.Value.w / 255.0f;
	//}

	ImGui::Unindent();
}

void fm::gui::introspectTransform(Transform& transform)
{
	ImGui::Text("Transform");
	ImGui::Indent();

	introspectVector3(transform.position, "Position");
	introspectVector3(transform.scale, "Scale");
	introspectVector3(transform.rotation, "Rotation");

	ImGui::DragFloat("Angle", &transform.angle, 1.0f, 0, 360);

	ImGui::Unindent();
}

void fm::gui::introspectVector3(Vector3 & vector, std::string label)
{
	std::string xId = "x##" + label;
	std::string yId = "y##" + label;
	std::string zId = "z##" + label;

	ImGui::Columns(4, nullptr, false);

	ImGui::InputFloat(xId.c_str(), &vector.x, 0, 0);
	ImGui::NextColumn();

	ImGui::InputFloat(yId.c_str(), &vector.y);
	ImGui::NextColumn();

	ImGui::InputFloat(zId.c_str(), &vector.z);
	ImGui::NextColumn();

	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::Columns(1, nullptr, false);
}

void fm::gui::startGui(int width, int height)
{
	// Perform setup for ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2{ (float)width, (float)height };
	io.RenderDrawListsFn = onRenderDrawLists;

	// Setup keyboard
	io.KeyMap[ImGuiKey_Tab]				= 9;                 
	io.KeyMap[ImGuiKey_LeftArrow]		= GLUT_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow]		= GLUT_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow]			= GLUT_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow]		= GLUT_KEY_DOWN;
	io.KeyMap[ImGuiKey_Home]			= GLUT_KEY_HOME;
	io.KeyMap[ImGuiKey_End]				= GLUT_KEY_END;
	io.KeyMap[ImGuiKey_Delete]			= 127;
	io.KeyMap[ImGuiKey_Backspace]		= 8;
	io.KeyMap[ImGuiKey_Enter]			= 13;
	io.KeyMap[ImGuiKey_Escape]			= 27;
	io.KeyMap[ImGuiKey_A]				= 1;
	io.KeyMap[ImGuiKey_C]				= 3;
	io.KeyMap[ImGuiKey_V]				= 22;
	io.KeyMap[ImGuiKey_X]				= 24;
	io.KeyMap[ImGuiKey_Y]				= 25;
	io.KeyMap[ImGuiKey_Z]				= 26;

	// Load a default font into memory
	unsigned char* pixels;
	int fw, fh;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &fw, &fh);
	
	GLint last_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fw, fh, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	io.Fonts->TexID = (void*)(intptr_t)textureHandle;
	glBindTexture(GL_TEXTURE_2D, last_texture);
}

void fm::gui::updateGui(Input * input, float dt, int width, int height)
{
	// set the deltatime, input..
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = dt;
	io.DisplaySize = ImVec2{ (float)width, (float)height };
	
	io.MousePos = ImVec2{ (float)input->getMouseX(), (float)input->getMouseY() };
	io.MouseDown[0] = input->isLeftMouseButtonPressed();
	io.MouseDown[1] = input->isRightMouseButtonPressed();
	io.MouseWheel = input->scrollAmount();

	// begin a new frame
	ImGui::NewFrame();
}

void fm::gui::renderGui()
{
	// TODO switch states?
	ImGui::Render();
}

/*
* Credit to Elias Daler for saving me a headache or ten.
* https://github.com/eliasdaler/imgui-sfml/blob/master/imgui-SFML.cpp
*/
void fm::gui::onRenderDrawLists(ImDrawData* drawData)
{
	if (drawData->CmdListsCount == 0) {
		return;
	}

	ImGuiIO& io = ImGui::GetIO();
	assert(io.Fonts->TexID != NULL); // You forgot to create and set font texture

	// scale stuff (needed for proper handling of window resize)
	int fb_width = static_cast<int>(io.DisplaySize.x * io.DisplayFramebufferScale.x);
	int fb_height = static_cast<int>(io.DisplaySize.y * io.DisplayFramebufferScale.y);
	if (fb_width == 0 || fb_height == 0) { return; }
	drawData->ScaleClipRects(io.DisplayFramebufferScale);

#ifdef GL_VERSION_ES_CL_1_1
	GLint last_program, last_texture, last_array_buffer, last_element_array_buffer;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
#else
	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
#endif

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

#ifdef GL_VERSION_ES_CL_1_1
	glOrthof(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, +1.0f);
#else
	glOrtho(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, +1.0f);
#endif

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	for (int n = 0; n < drawData->CmdListsCount; ++n) {
		const ImDrawList* cmd_list = drawData->CmdLists[n];
		const unsigned char* vtx_buffer = (const unsigned char*)&cmd_list->VtxBuffer.front();
		const ImDrawIdx* idx_buffer = &cmd_list->IdxBuffer.front();

		glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer + offsetof(ImDrawVert, pos)));
		glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer + offsetof(ImDrawVert, uv)));
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void*)(vtx_buffer + offsetof(ImDrawVert, col)));

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size(); ++cmd_i) {
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback) {
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else {
				GLuint tex_id = (GLuint)*((unsigned int*)&pcmd->TextureId);
				glBindTexture(GL_TEXTURE_2D, tex_id);
				glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w),
					(int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, GL_UNSIGNED_SHORT, idx_buffer);
			}
			idx_buffer += pcmd->ElemCount;
		}
	}
#ifdef GL_VERSION_ES_CL_1_1
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
	glDisable(GL_SCISSOR_TEST);
#else
	glPopAttrib();
#endif

}

void fm::gui::onKeyDown(char key)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[key] = true;
	io.AddInputCharacter(key);
}

void fm::gui::onKeyUp(char key)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[key] = false;
}

void fm::gui::debugInput(Input * input, float dt)
{
	static bool showMetrics = false;

	// render input demo gui
	if (ImGui::Begin("Debug")) {
		// check if we're showing metrics or not
		ImGui::Checkbox("Show Gui Metrics", &showMetrics);
		
		if(showMetrics)
			ImGui::ShowMetricsWindow(&showMetrics);

		// show dt
		std::string dt_str = std::to_string(dt);
		ImGui::LabelText("dt", dt_str.c_str());

		// 1 for pressed, 0 for not
		char* lPressed = (input->isLeftMouseButtonPressed()) ? "1" : "0";
		ImGui::LabelText("LEFT", lPressed);

		// 1 for pressed, 0 for not
		char* rPressed = (input->isRightMouseButtonPressed()) ? "1" : "0";
		ImGui::LabelText("RIGHT", rPressed);

		// mouse scroll
		std::string scroll = std::to_string(input->scrollAmount());
		ImGui::LabelText("SCROLL", scroll.c_str());

		// mouse x, y
		ImGui::LabelText("MOUSE X", std::to_string(input->getMouseX()).c_str());
		ImGui::LabelText("MOUSE Y", std::to_string(input->getMouseY()).c_str());

		ImGui::End();
	}
}

void fm::gui::endGui()
{
	ImGui::Shutdown();
}

#endif