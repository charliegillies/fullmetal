/*
 * Fullmetal is a set of utilities for OpenGL, including a 
 * node-driven scene graph. The node graph has features like:
 * Handling parent-child matrix relationships.
 * Shapes such as cubes, spheres, polygons. 
 * Lights including ambient, spot and directional. 

 * TODO:
	* Find a way to dynamically handle light id assignment (0-7)
	* Possibly implement a simple ImGui for viewing the nodes.
	* Possibly find a way to read/write the nodes to/from JSON. 
	* Implement a camera with utilities. NOT a scene node.
	* Allow deletion of nodes in the scene graph, including children.
	* Implement sorting for SceneNodes so we can put lighting first.
	* Draw normals for custom shapes (quad, tri, etc)

 * GitHub Repo: ??
 * Author: Charlie Gillies
 * Start Date: 19.09.2017
 */

#ifndef FULLMETAL_H
#define FULLMETAL_H

#include "fullmetal-config.h"
#include <vector>
#include <string>

namespace fm {

	struct Color;
	class Vector3;
	class Input;
	class Transform;
	class SceneNodeGraph;
	class SceneNode;

	/*
	 * Shortcut to call glColor4f(color.r, color.g, color.b, color.a).
	 */
	void fmApplyColor(Color& color);

	/*
	 * Shortcut to apply the transform in OpenGL.
	 */
	void fmApplyTransform(Transform& transform);

	/*
	 * Shortcut to call glNormal(normal.x, normal.y, normal.z) and glVertex(x, y, z).
	 */
	void fmNormalVertex(const Vector3& normal, float x, float y, float z);

	/*
	 * Utility to help handle 3D space.
	 * @author Paul Robertson
	 */
	class Vector3 {
	public:
		Vector3(float x, float y, float z);
		Vector3();
		Vector3 copy();

		void set(float x, float y, float z);
		void setX(float x);
		void setY(float y);
		void setZ(float z);

		float getX();
		float getY();
		float getZ();

		void add(const Vector3& v1, float scale = 1.0);
		void subtract(const Vector3& v1, float scale = 1.0);
		void scale(float scale);

		float dot(const Vector3& v2);
		Vector3 cross(const Vector3& v2);

		void normalise();
		Vector3 normalised();
		float length();
		float lengthSquared();

		bool equals(const Vector3& v2, float epsilon);
		bool equals(const Vector3& v2);

		Vector3 operator+(const Vector3& v2);
		Vector3 operator-(const Vector3& v2);

		Vector3& operator+=(const Vector3& v2);
		Vector3& operator-=(const Vector3& v2);

		float x;
		float y;
		float z;
	};

	/* Input class
	 * Stores current keyboard and mouse state include, pressed keys, mouse button pressed and mouse position.
	 * @author Paul Robertson, Charlie Gillies
	 */
	class Input
	{
		// Mouse struct stores mouse related data include cursor
		// x, y coordinates and left/right button pressed state.
		struct Mouse
		{
			Mouse();
			int x, y;
			bool left, right;
			float scrollDirection;
		};

		// The state of the mouse and keyboard for a specific frame
		struct State {
			Mouse mouse;
			bool keys[256];
		};

	public:
		// Getters and setters for keys
		void SetKeyDown(unsigned char key);
		void SetKeyUp(unsigned char key);
		bool isKeyDown(int);

		// getters and setters for mouse buttons and position.
		void setMouseX(int);
		void setMouseY(int);
		void setMousePos(int x, int y);
		int getMouseX();
		int getMouseY();

		void setLeftMouseButton(bool b);
		void setRightMouseButton(bool b);
		bool isLeftMouseButtonPressed();
		bool isRightMouseButtonPressed();

		// Set the scroll amount (1.0 for down, -1.0 for up, 0.0 for nothing)
		void setScrolling(float scrollAmount);
		// Get the scroll amount
		float scrollAmount();

	private:
		// The mouse/kb state of this frame
		State frameState;
	};

	/*
	 * Transform is the rotation, scale and position of an object.
	 */
	class Transform {
	public:
		Transform();
		Transform(Vector3& position, Vector3& scale, Vector3& rotation);
		Transform(Vector3& position, Vector3& scale, Vector3& rotation, float angle);

		/*
		 * Position in 3D space of the object.
		 */
		Vector3 position;

		/*
		 * Scale of the object in 3D space.
		 */
		Vector3 scale;

		/*
		 * Defines what axis the transform is rotating on.
		 */
		Vector3 rotation;

		/*
		 * Defines the angle of the actual object.
		 */
		float angle;

		/*
		 * Changes the transform.angle property by the given amount.
		 */
		void rotate(float amount);

		/*
		 * Moves the angle by the x, y and y axis of the given vector.
		 */
		void move(Vector3& v);

		/*
		 * Moves the object by the x and y axis.
		 */
		void move(float x, float y);

		/*
		 * Moves the object by the x, y and z axis.
		 */
		void move(float x, float y, float z);
	};

	/*
	 * Represents an opengl color.
	 */
	struct Color {
		Color(float r, float g, float b, float a);
		Color(float r, float g, float b);
		Color();

		void clamp(float& v);

		float r;
		float g;
		float b;
		float a;
	};

	/*
	 * A triangle made out of three vertex points in 3D space.
	 */
	struct Tri {
		Tri();
		Tri(Vector3 v1, Vector3 v2, Vector3 v3);

		Vector3 v1;
		Vector3 v2;
		Vector3 v3;
	};

	/*
	 * Holds the scene nodes.
	 */
	class SceneNodeGraph {
	private:
		std::vector<SceneNode*> _nodes;

	public:
		~SceneNodeGraph();

		/*
		 * Adds a node to the graph.
		 */
		SceneNode* addNode(SceneNode* node);

		/*
		 * Calls .Render() on all the scene nodes.
		 */
		void render();

		/*
		 * Gets the number of nodes inside of the node graph.
		 * Includes count of child nodes.
		 */
		int nodeCount();

		/*
		 * Gets all the nodes inside of the graph.
		 */
		std::vector<SceneNode*>& getNodes();
	};

	/*
	 * A scene node is an item for the scene graph.
	 */
	class SceneNode {
	private:
		unsigned int _uid;

	public:
		SceneNode();

		~SceneNode();

		/*
		 * Name of the scene node.
		 * Either default according to the scene node type
		 * or manually set by the user.
		 */
		std::string name;

		/*
		 * The transform of the node. The scene node owns this object
		 * and will delete it at the end of SceneNode's lifetime.
		 */
		Transform transform;

		/*
		 * The child nodes of this scene node.
		 * All of these nodes will be deleted when their parent is.
		 */
		std::vector<SceneNode*> childNodes;

		/*
		 * Render the scene node.
		 */
		virtual void render();

		/*
		 * Adds a child to the scene node.
		 */
		void addChild(SceneNode* child);

		/*
		 * Gets an integer id unique to the scene node.
		 */
		int getUniqueId();

		/*
		 * Returns a count of this nodes children and all of their nodes children.
		 */
		int childCount();

#if FM_EDITOR
		virtual void introspect();
#endif
	};

	/*
	 * Abstract base class for a shape node.
	 */
	class ShapeNode : public SceneNode {
	public:
		ShapeNode(Color color);

		/*
		 * The color that will be drawn.
		 */
		Color color;

#if FM_EDITOR
		void introspect() override;
#endif
	};

	/*
	 * The cube node, derives from ShapeNode.
	 */
	class CubeNode : public ShapeNode {
	public:
		CubeNode(Color color);
		CubeNode();

		void render() override;

#if FM_EDITOR
		void introspect() override;
#endif
	};

	/*
	 * Sphere node, derives from ShapeNode.
	 */
	class SphereNode : public ShapeNode {
	public:
		SphereNode(Color color);
		SphereNode();

		void render() override;

#if FM_EDITOR
		void introspect() override;
#endif
	};

	/*
	 * Plane node, derives from ShapeNode.
	 * Planes are made of triangles, denoted by the QuadSize,
	 * width and height param given in the constructor.
	 */
	class PlaneNode : public ShapeNode {
	public:
		PlaneNode(Color color, int quadSize, int width, int height);
		PlaneNode();

		void render() override;

#if FM_EDITOR
		void introspect() override;
#endif

		int quadLength();
		int width();
		int height();
		void buildQuads(int size, int width, int height);

	private:
		int _quadSize;
		int _width;
		int _height;
		std::vector<Tri> _tris;
	};

	/*
	 * Abstract base class for lighting nodes.
	 */
	class LightNode : public SceneNode {
	public:
		Color color;

		LightNode(Color color);

#if FM_EDITOR
		void introspect() override;
#endif
	};

	/*
	 * Ambient light node, derives from LightNode.
	 */
	class AmbientLightNode : public LightNode {
	public:
		AmbientLightNode(Color color);
		AmbientLightNode(Color lightColor, Color diffuseColor);
		AmbientLightNode();

		Color diffuse;

		void render() override;

#if FM_EDITOR
		void introspect() override;
#endif
	};

	/*
	 * Directional light node, derives from LightNode.
	 * Creates light of a certain colour from it's transform.position.
	 */
	class DirectionalLightNode : public LightNode {
	public:
		DirectionalLightNode(Color color);
		DirectionalLightNode();

		void render() override;

#if FM_EDITOR
		void introspect() override;
#endif
	};

	/*
	 * Spot light node, derives from Light Node.
	 * Creates light of a certain colour from
	 */
	class SpotLightNode : public LightNode {
	public:
		SpotLightNode(Color lightColor, Color diffuseColor, Vector3 direction);
		SpotLightNode();

		Vector3 direction;
		Color diffuse;

		float cutoff;
		float exponent;

		void render() override;

#if FM_EDITOR
		void introspect() override;
#endif
	};
}
#endif