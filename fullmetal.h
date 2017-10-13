/*
 * Fullmetal is a set of utilities for OpenGL, including a 
 * node-driven scene graph. The node graph has features like:
 * Handling parent-child matrix relationships.
 * Shapes such as cubes, spheres, polygons. 
 * Lights including ambient, spot and directional. 

 * TODO:
	* Find a way to dynamically handle light id assignment (0-7)
	* Implement a camera with utilities. NOT a scene node.
	* Implement sorting for SceneNodes so we can put lighting first.

 * GitHub Repo: https://github.com/charliegillies/fullmetal
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
	class Camera;
	class SceneNodeGraph;
	class SceneNode;
	struct Material;
	struct ObjModel;

	void clamp(int& value, int min, int max);
	void clamp(float& value, float min, float max);

	/*
	 * Shortcut to call glColor4f(color.r, color.g, color.b, color.a).
	 */
	void applyColor(Color& color);

	/*
	 * Shortcut to apply a material in OpenGL.
	 */
	void applyMaterial(Material& material);

	/*
	 * Shortcut to apply the transform in OpenGL.
	 */
	void applyTransform(Transform& transform);

	/*
	 * Shortcut to call glNormal(normal.x, normal.y, normal.z) and glVertex(x, y, z).
	 */
	void normalVertex(const Vector3& normal, float x, float y, float z);

	/* 
	 * Helper for removing a specific scene node from a vector of scene nodes.
	 */
	bool removeNodeFromVector(SceneNode* node, std::vector<SceneNode*>& nodes);

	void moveNodeUpHierarchy(SceneNode* node, SceneNodeGraph* graph);

	void moveNodeDownHierarchy(SceneNode* node, SceneNodeGraph* graph);

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
	 * Handles the viewport, frustrum, etc.
	 */
	class Camera {
	public:
		Camera();

		/*
		 * 
		 */
		Transform transform;

		/*
		 * 
		 */
		void view();
	};

	/*
	 * Represents an opengl color.
	 */
	struct Color {
		Color(float r, float g, float b, float a);
		Color(float r, float g, float b);
		Color();

		float r;
		float g;
		float b;
		float a;
	};

	/*
	 * Represents the material of an object.
	 */
	struct Material {
		Material();
		
		/*
		 * The overall colour of the object, is effected by the ambient light in the scene.
		 */
		Color ambientColor;
		
		/*
		 * Interacts with the light where the object is lit.
		 */
		Color diffuseColor;
		
		/* 
		 *   
		 */
		Color specularColor;

		/* 
		 * 
		 */
		float shininess;

		/* 
		 * If true, draws on both the FRONT and BACK poly faces.
		 */
		bool doubleSided;

		/*
		 * If true, enables specular reflection with the specularColor property.
		 */
		bool specularEnabled;

		/*
		 * If true, enables the shininess openGL property with the shiness material value.
		 */
		bool shininessEnabled;
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

		/*
		 * Removes a node from the top-level of the scene graph.
		 */
		void removeNode(SceneNode* node);
	};

	/*
	 * A scene node is an item for the scene graph.
	 */
	class SceneNode {
	private:
		unsigned int _uid;
		SceneNode* _parent;

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
		 * If the node is enabled in the scene or not.
		 * If not enabled - the node and it's children will not render.
		 */
		bool enabled;

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

		/*
		 * Gets the parent node of the scene node.
		 * Returns a nullptr if node is top-level.
		 */
		SceneNode* getParent();

		/*
		 * Removes a child from the child nodes.
		 */
		SceneNode* removeChild(SceneNode* child);
	};

	/*
	 * Abstract base class for a shape node.
	 */
	class ShapeNode : public SceneNode {
	public:
		ShapeNode(Color color);

		/*
		 * The material of the shape.
		 */
		Material material;
	};

	/*
	 * The cube node, derives from ShapeNode.
	 */
	class CubeNode : public ShapeNode {
	public:
		CubeNode(Color color);
		CubeNode();

		void render() override;
	};

	/*
	 * Sphere node, derives from ShapeNode.
	 */
	class SphereNode : public ShapeNode {
	private:
		int _slices;
		int _stacks;

	public:
		SphereNode(Color color);
		SphereNode();

		/* */
		int& getSlices();
		
		/* */
		int& getStacks();

		void render() override;
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
	};

	/*
	 * Spot light node, derives from Light Node.
	 * Creates light of a certain colour.
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
	};

	/*
	 * Draws the mesh of a loadable object.
	 */
	class MeshNode : public SceneNode {
	public:
		ObjModel* model;
		Material material;

		MeshNode();
		MeshNode(std::string modelPath);

		void render() override;
	};
}

#endif