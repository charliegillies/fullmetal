# fullmetal
A simple extensible scene graph / level design utility for OpenGL with (optional) extendable GUI and JSON IO.

Right now this is still in development, please check 'to do' for the list of upcoming features.



## features
* Scene graph that uses 'SceneNodes'. Nodes can have children which forms a tree data structure.
* Children are always rendered within the parents transformation matrix, so child positions are always relative to parent positions.
* Optional inspector GUI that shows you the scene graph tree.
* Introspection that allows editing of scene nodes inside of the inspector.
* Create and destroy scene nodes inside of the inspector.
* Optional I/O handling for the scene nodes, allows you to read/write the scene nodes to JSON.
* Default scene nodes are spheres, quads, tris, cubes, planes, ambient lights, directional lights and spot lights.
* Easily extendable API.

## the folder structure
* The config file (fullmetal-config.h) tells the project what dependencies we are compiling with, it is here that you can turn off the IO and GUI features of fullmetal. 

* The core of fullmetal is in the fullmetal.h file. This is the basic openGL-dependency-only version of fullmetal, it contains the utilities - including the scene graph and the default nodes.

* The GUI of fullmetal exists primarily inside of the fullmetal-gui.h file. It is here that the ImGui code is defined for drawing the node graph and debug information.

* The IO of fullmetal is in the fullmetal-io.h file. It is here that the methods for reading and writing the scene graph and nodes, are defined.

* The introspector code of fullmetal is inside fullmetal-introspectors.h. This is where the nodes properties are drawn for editing in the inspector.

## api summary 

* The Scene Node: The base class of any node that exists inside the scene graph. A node must have only two things: a render method and a transform. The node must be responsible for rendering its children inside of the render method, relative to the its own matrix. Scene nodes must also be default constructible.

* Scene Node Graph: The scene graph is responsible for owning all of the top-parent scene nodes, it's essentially a wrapper around std::vector<SceneNode*> with basic helpers.

* Graph Render Config: Passed around the gui so various parts of the gui can manage the gui state without changing global variables or an internal state. You don't need to set anything here, just create an instance of it and pass it around the gui. 

* Node type table: This is turned on regardless of the IO/GUI config settings. It's essentially a system that maps a string to a node type using templates. Both the GUI and the IO use it. The GUI uses it to find all known node types, so it can add those nodes to the scene. The IO uses it for writing and reading nodes to/from JSON. Node key string values should NOT change once they have been defined.

* Using the graph gui: After you have your scene graph, node type table and graph render config you can make graph gui calls. This is as simple as including the fullmetal-gui file, and calling `fmgui::renderNodeGraph(nodeGraph, graphConfig, typeTable);`. If you don't want to be able to create nodes during the program runtime, you can pass typeTable in as nullptr. 

* Using the graph I/O: These are just helpers for reading/writing a scene graph to JSON. Writing is as simple as calling `fmio::writeSceneGraph(filepath, sceneGraph, typeTable)` and reading is as simple as calling `fmio::readSceneGraph(filepath, sceneGraph, typeTable)`. You can easily extend the API for writing 3rd party types.

## extending the api
* Creating a new scene node, including the introspection method:

```
#include "fullmetal.h"
#include "fullmetal-config.h"
#include "fullmetal-gui.h"

// We create a new scene node type putting the code 
// inside the .h file for example purposes only
class TriangleNode : public SceneNode { 
private:
	Tri _tri;

public:
	TriangleNode() : SceneNode() {
		// default name that will show in the graph gui
		name = "Triangle Node";

		// define some example vec3 points for _tri
		_tri.v1 = Vector3(1, 1, 1);
		_tri.v2 = Vector3(2, 2, 2);
		_tri.v3 = Vector3(3, 3, 3); 
	}

	void render() override { 
		glPushMatrix();

		// applies translation, scale and rotation to the matrix
		glApplyTransform(transform);

		// draws our triangle
		drawTri(_tri);

		// renders any children we may have
		SceneNode::render();

		glPopMatrix();
	}

	// The introspect method is called by the gui once
	// the node has been selected by the user
	// and it is for editing the internals of the node
	// during the programs runtime.
#ifdef FM_EDITOR
	void introspect() override { 
		// draw tri v1 to v3
		fmgui::introspectVector3(_tri.v1, "v1");
		fmgui::introspectVector3(_tri.v2, "v2");
		fmgui::introspectVector3(_tri.v3, "v3");
	}
#endif
};
```

* Registering that scene node to the type table

```
#include "fullmetal-types.h"

NodeTypeTable* createNewTypeTable() { 
	// creates a type table with the default fullmetal nodes
	NodeTypeTable* oldTable = createDefaultTypeTable();

	// register our new node with the id "TriangleNode"
	oldTable.registerNode<TriangleNode>("TriangleNode");

	return oldTable;
}
```
Now just like that, our GUI will show the TriangleNode in the editor - and the IO will have an ID for parsing the node from JSON.

* Setting up the I/O for the new node..
```
// TODO
```

## building fullmetal

This section will be complete after version 1 is ready. There will be VS + CMAKE build support.

## todo
* Implement an FBX loader for loading and displaying 3d models.

* Implement a (proper) camera system and controls for moving around the scene.

* Allow sorting of the nodes, so we can prioritize render order - for doing things like rendering lights before the geometry.

* Compress JSON file output

* Finish docs

* Write example programs

* Create a makefile with dependency options

## project dependencies
* (Easily replaceable) Rendering current depends on GLUT, but this can be replaced easily enough with other bindings as it doesn't use any real GLUT specific features.

* (Optional, Included) project has included the popular [c++ gui library 'ImGui'](https://github.com/ocornut/imgui) and has support for rendering and extending the main UI. This dependency can be turned off with a single line of code.

* (Optional, Included) project has included the popular [c++ json libary 'nlohmann json'](https://github.com/nlohmann/json), and uses that for handling the IO of the scene graph. This dependency can be turned off with a single line of code.

* OpenGL

* C++ 11 or above

## purpose

This was written to make my life easier in Graphics Programming for University. It's not a commercial tool, but since I'm using it for University it has various limitations as there are libraries that I would have liked to have use but sadly cannot.
