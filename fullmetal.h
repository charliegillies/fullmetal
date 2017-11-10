/*
 * Fullmetal is a set of utilities for OpenGL, including a 
 * node-driven scene graph. The node graph has features like:
 * Handling parent-child matrix relationships.
 * Primitive shapes such as cubes, spheres, polygons. 
 * Basic lights including ambient, spot and directional.
 * Parsing obj files, materials, textures, etc.

 * It also comes with a JSON I/O and front-end gui
 * for editing the graph at runtime.

 * TODO:
	* Implement a gizmo system for drawing normals, cameras, lights, etc.

 * GitHub Repo: https://github.com/charliegillies/fullmetal
 * Author: Charlie Gillies
 * Start Date: 19.09.2017
 * End Date: ? 
 */

#ifndef FULLMETAL_H
#define FULLMETAL_H

#include "fullmetal-config.h"
#include <vector>
#include <string>
#include <map>

namespace fm {

	// Categories define in what order the nodes will be rendered
	const int LIGHT_CATEGORY = -1;
	const int DEFAULT_NODE_CATEGORY = 1;

	struct Color;
	class Vector3;
	class Input;
	class Transform;
	class Camera;
	class SceneNodeGraph;
	class SceneNode;
	struct Texture;
	struct Material;
	struct ObjModel;
	class AssetManager;

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
	 * Shortcut to apply a texture through a material.
	 * Returns true/false depending if the texture was applied.
	 */
	bool applyTexture(Material& material);

	/*
	 * Calls glNormal3f, glTexcoord2f and glVertex3f in order.
	 */
	void normUvVert(float nX, float nY, float nZ, float uvx, float uvy, float vx, float vy, float vz);

	/*
	 * Shortcut to call glNormal(normal.x, normal.y, normal.z) and glVertex(x, y, z).
	 */
	void normalVertex(const Vector3& normal, float x, float y, float z);

	/* 
	 * Helper for removing a specific scene node from a vector of scene nodes.
	 */
	bool removeNodeFromVector(SceneNode* node, std::vector<SceneNode*>& nodes);

	/*
	 * Handles the cloning of a node.
	 */
	void cloneNode(SceneNodeGraph* graph, SceneNode* node);

	/*
	 * Gets a dynamic value between GL_LIGHT0 and GL_LIGHT7
	 * So every light in the scene can use a dynamic ID.
	 */
	int createDynamicLightId();

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

		bool isZero();

		void normalise();
		Vector3 normalised();
		float length();
		float lengthSquared();

		bool equals(const Vector3& v2, float epsilon);
		bool equals(const Vector3& v2);

		Vector3 operator+(const Vector3& v2);
		Vector3 operator-(const Vector3& v2);

		Vector3 operator+(const float& v);
		Vector3 operator-(const float& v);

		Vector3 operator*(const Vector3& v2);
		Vector3 operator*(const float& scalar);

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
	private:
		// directional vectors, calculated in every rotation change
		Vector3 _forward, _forwardTarget, _up, _right;
		// the position & orientation of the camera
		Vector3 _position, _rotation;
		// if rotation or position needs to be recalculated
		bool _dirty;
		// screen size
		int _screenW, _screenH;

		// calculates the directions
		void calculateDirections();

	public:
		Camera(int screenW, int screenH);

		/** Call on the screen resize event. */
		void onScreenResize(int w, int h);

		/** Rotates on x axis. */
		void pitch(float p);

		/** Rotates on y axis. */
		void yaw(float y);

		/** Sets position of the camera. */
		void setPosition(const Vector3& pos);

		/** Sets the orientation of the camera. */
		void setOrientation(const Vector3& orientation);

		/** Offsets the camera position by the given Vector3 */
		void move(Vector3 offset);

		/** Calculates the directions, if there's been a change. */
		void update();

		/** Applies the camera view. */
		void view();

		/** Resets the position and orientation of the camera to default. */
		void reset();

		/** Gets the middle of the screen on X. */
		int getCentreX();

		/** Gets the middle of the screen on Y. */
		int getCentreY();

		/** Gets the current position of the camera. */
		const Vector3& getPosition();

		/** Gets the rotation, yaw/pitch/roll orientation.*/
		const Vector3& getRotation();

		/** Up direction from the camera transform. */
		Vector3 up();

		/** Down direction from camera transform. */
		Vector3 down();

		/** Forward direction from camera transform. */
		Vector3 forward();

		/** Backward direction from camera transform.*/
		Vector3 back();

		/** Left direction from camera transform. */
		Vector3 left();

		/** Right direction from camera transform. */
		Vector3 right();
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
	 * The texture data that is loaded via SOIL.
	 */
	struct TextureData {
		TextureData();

		/*
		 * The id of the loaded texture, assigned using SOIL for use in OpenGL.
		 * If this id is 0, loading failed.
		 */
		int glTextureId;
		
		/*
		 * The filepath of the texture that is being loaded. 
		 * This is used for the editor and io more than anything else.
		 */
		std::string filepath;
	};

	/*
	 * Wrapper around texture data with specific information that
	 * will be used by OpenGL for materials, 3d models, etc.
	 */
	struct Texture {
		/*
		 * Creates an empty texture.
		 */
		Texture();
		
		/*
		 * Creates a texture, attempts to load texture data via the filepath given.
		 */
		Texture(const std::string& fp);

		/*
		 * Data that was loaded from the img using SOIL.
		 * Is a nullptr by default.

		 * Note: TextureData is owned by the asset manager, so the texture 
		 * object will not delete it at the end of Textures own lifetime.
		 */
		TextureData* data;
	};

	/*
	 * The owner of assets in the game scene.
	 * This is the centralized area where assets will be created and deleted. 
	 * The asset manager "owns" the assets that are loaded in the scene.

	 * We could have used a 'smart' template design here, but all that would have done
	 * is increased loading time and made the code harder to read.
	 */
	class AssetManager {
	private:
		std::map<const std::string, TextureData*> _loadedTxData;
		std::map<const std::string, ObjModel*> _loadedModelData;

		// forces access through instance
		AssetManager();
		~AssetManager();

	public:
		/* Global instance of the asset manager. */
		static AssetManager* global;

		/* Gets the cached version of texture data or loads a new one. */
		TextureData* getTextureData(const std::string& fp);
		
		/* Gets the cached version of the ObjModel or loads a new one. */
		ObjModel* getObjModel(const std::string& fp);
	};

	/*
	 * Represents the material of an object.
	 */
	struct Material {
		Material();
		~Material();

		/*
		 * 
		 */
		Texture* texture;

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

	protected:
		int nodeCategory;

	public:
		SceneNode();
		SceneNode(SceneNode* node);

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
		* Creates a clone of the scene node.
		*/
		virtual SceneNode* clone() = 0;

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

		/*
		 * What node this category is in.
		 * Determines render order in the scene.
		 */
		int category();
	};

	/*
	 * Abstract base class for a shape node.
	 */
	class ShapeNode : public SceneNode {
	public:
		ShapeNode(Color color);
		ShapeNode(ShapeNode* node);

		SceneNode* clone() override;

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
		CubeNode(CubeNode* node);

		SceneNode* clone() override;

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
		SphereNode(SphereNode* node);

		SceneNode* clone() override;

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
		PlaneNode(PlaneNode* node);

		SceneNode* clone() override;

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
		std::vector<Tri> _uvs;
	};

	/*
	 * Abstract base class for lighting nodes.
	 */
	class LightNode : public SceneNode {
	protected:
		int lightId;

	public:
		Color color;

		LightNode(Color color);
		LightNode(LightNode* node);
	};

	/*
	 * Ambient light node, derives from LightNode.
	 */
	class AmbientLightNode : public LightNode {
	public:
		AmbientLightNode(Color color);
		AmbientLightNode(Color lightColor, Color diffuseColor);
		AmbientLightNode();
		AmbientLightNode(AmbientLightNode* node);

		Color diffuse;

		void render() override;
		SceneNode* clone() override;
	};

	/*
	 * Directional light node, derives from LightNode.
	 * Creates light of a certain colour from it's transform.position.
	 */
	class DirectionalLightNode : public LightNode {
	public:
		DirectionalLightNode(Color color);
		DirectionalLightNode();
		DirectionalLightNode(DirectionalLightNode* node);

		void render() override;
		SceneNode* clone() override;
	};

	/*
	 * Spot light node, derives from Light Node.
	 * Creates light of a certain colour.
	 */
	class SpotLightNode : public LightNode {
	public:
		SpotLightNode(Color lightColor, Color diffuseColor, Vector3 direction);
		SpotLightNode();
		SpotLightNode(SpotLightNode* node);

		Vector3 direction;
		Color diffuse;

		float cutoff;
		float exponent;

		void render() override;
		SceneNode* clone() override;
	};

	/*
	 * Draws the mesh of a loadable object.
	 */
	class MeshNode : public SceneNode {
	public:
		ObjModel* model;
		Material material;

		MeshNode();
		MeshNode(const std::string& modelPath);
		MeshNode(MeshNode* node);

		void render() override;
		SceneNode* clone() override;
	};

	/*
	 * A cylinder is a type of shape node that is generated in code.
	 */
	class CylinderNode : public ShapeNode {
	private:
		std::vector<float> _vertices;
		std::vector<float> _normals;
		std::vector<float> _uvs;

		void pushVertUv(float x, float y, float z, float u, float v);
	
		int _numSegments;

	public:
		CylinderNode();
		CylinderNode(CylinderNode* node);

		int numSegments();
		void build(int segments);

		void render() override;
		SceneNode* clone() override;
	};
}

#endif