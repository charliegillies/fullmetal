/*
 *  Set of GUI tools for drawing our nodes using ImGui.
 */
#pragma once
// Import the config that includes our compile options
#include "fullmetal-config.h"

// Ensure that the editor is turned on, (if you want it)
// otherwise we will not compile the gui code.
#ifdef FM_EDITOR
#include <vector>

struct ImDrawData;

namespace fm {
	class NodeTypeTable;
	class Input;
	class SceneNodeGraph;
	class SceneNode;
	class Transform;
	struct Vector3;
	struct Color;

	namespace gui {
		/*
		 * Config for the graph render method.
		 */
		struct GraphRenderConfig {
			/*
			 * If the window is toggled on or not.
			 */
			bool window_toggled;

			/*
			 * The node that has been selected, if any.
			 */
			SceneNode* selected_node;

			/*
			 * The filepath to the file that is being written/read from, if any.
			 */
			std::string filepath;

			GraphRenderConfig();
		};

		/*
		 * Renders a node graph.
		 * Must be called after updateGui() but before renderGui().
		 * Param 'typeTable' can be null, if it is then no add node options will draw.
		 */
		void drawNodeGraph(SceneNodeGraph* nodeGraph, GraphRenderConfig* config, NodeTypeTable* typeTable);

		/*
		 * Renders the add node options from the node type table.
		 */
		void drawAddNodeOptions(SceneNodeGraph* nodeGraph, GraphRenderConfig* graphConfig, NodeTypeTable* _nodeTypeTable);

		/*
		 * Deletes a node from the graph.
		 */
		void deleteNodeFromGraph(fm::gui::GraphRenderConfig * graphConfig, fm::SceneNodeGraph * nodeGraph);

		/*
		 * ImGui shortcut to draw a combo box with a vector of std::strings.
		 */
		void drawComboBox(std::string title, std::vector<std::string>& items, int& index);

		/*
		 * Renders the scene nodes. Called by renderNodeGraph.
		 */
		void drawNodes(std::vector<SceneNode*>& nodes, GraphRenderConfig* config);

		/* 
		 * Draws an input string field with ImGui, using the modern std::string.
		 */
		void guiString(std::string& str, std::string label, int bufSize = 256);

		/*
		 * Renders a specific scene node.
		 * Recursively calls itself for the given nodes children.
		 */
		SceneNode* drawNodeSelect(SceneNode* node, GraphRenderConfig* config);

		/*
		 * Introspects a color.
		 */
		void introspectColor(Color& color, std::string title = "Color");

		/*
		 * Renders the transform.
		 */
		void introspectTransform(Transform& transform);

		/*
		 * Renders a vector3 (x, y, z) using ImGui
		 */
		void introspectVector3(Vector3& vector, std::string label);

		/*
		 * Initializes the gui.
		 */
		void startGui(int windowWidth, int windowHeight);

		/*
		 * Updates the gui.
		 */
		void updateGui(Input* input, float dt, int width, int height);

		/*
		 * Begins the render of the gui.
		 */
		void renderGui();

		/*
		 * Callback function for rendering imgui data.
		 */
		void onRenderDrawLists(ImDrawData* drawData);

		/*
		 *
		 */
		void onKeyDown(char key);

		/*
		 *
		 */
		void onKeyUp(char key);

		/*
		 *
		 */
		void debugInput(Input* input, float dt);

		/*
		 * Cleans up anything left by starting the gui.
		 */
		void endGui();
	}
}

#endif