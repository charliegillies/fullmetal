#include "fullmetal.h"
#include "fullmetal-3d.h"
#include "glut.h"

#include <math.h>
#include <cassert>
#include <algorithm>

// Includes for OpenGL go here
#include <gl/GL.h>
#include <gl/GLU.h>
#include "../SOIL.h"

void fm::clamp(int & value, int min, int max)
{
	if (value < min)
		value = min;
	else if (value > max)
		value = max;
}

void fm::clamp(float & value, float min, float max)
{
	if (value < min)
		value = min;
	else if (value > max)
		value = max;
}

// GL HELPERS
void fm::applyColor(Color& color)
{
	glColor4f(color.r, color.g, color.b, color.a);
}

void fm::applyMaterial(Material & mat)
{
	// If we're drawing double sided, draw on the front and the back
	auto polyMode = mat.doubleSided ? GL_FRONT_AND_BACK : GL_FRONT;

	// Apply the ambient color of the material 
	float ambientColors[4] = { mat.ambientColor.r, mat.ambientColor.g, mat.ambientColor.b, mat.ambientColor.a };
	glMaterialfv(polyMode, GL_AMBIENT, ambientColors);

	// Apply the diffuse color of the material
	float diffuseColors[4] = { mat.diffuseColor.r, mat.diffuseColor.g, mat.diffuseColor.b, mat.diffuseColor.a };
	glMaterialfv(polyMode, GL_DIFFUSE, diffuseColors);

	// Apply the specular color, if enabled.
	if (mat.specularEnabled) {
		float specularColor[4] = { mat.specularColor.r, mat.specularColor.g, mat.specularColor.b, mat.specularColor.a };
		glMaterialfv(polyMode, GL_SPECULAR, specularColor);
	}

	// Apply the shininess, if enabled.
	if (mat.shininessEnabled) {
		float shininess[1] = { mat.shininess };
		glMaterialfv(polyMode, GL_SHININESS, shininess);
	}
}

void fm::applyTransform(Transform& transform)
{
	glRotatef(transform.angle, transform.rotation.x, transform.rotation.y, transform.rotation.z);
	glTranslatef(transform.position.x, transform.position.y, transform.position.z);
	glScalef(transform.scale.x, transform.scale.y, transform.scale.z);
}

bool fm::applyTexture(Material & material)
{
	// Check if we're using a texture, if so, bind it!
	Texture* texture = material.texture;

	if (texture != nullptr && texture->data != nullptr) {
		// Linear filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Bind the texture using the loaded texture id
		glBindTexture(GL_TEXTURE_2D, material.texture->data->glTextureId);

		// Indicate that we're using the texture
		return true;
	}

	return false;
}

void fm::normUvVert(float nx, float ny, float nz, float uvx, float uvy, float vx, float vy, float vz)
{
	glNormal3f(nx, ny, nz);
	glTexCoord2f(uvx, uvy);
	glVertex3f(vx, vy, vz);
}

void fm::normalVertex(const Vector3 & normal, float x, float y, float z)
{
	glNormal3f(normal.x, normal.y, normal.z);
	glVertex3f(x, y, z);
}

bool fm::removeNodeFromVector(SceneNode * node, std::vector<SceneNode*>& _nodes)
{
	// try to find the node
	auto r = std::find(_nodes.begin(), _nodes.end(), node);

	if (r != _nodes.end()) {
		_nodes.erase(r);
		return true;
	}

	return false;
}

void fm::cloneNode(SceneNodeGraph * graph, SceneNode * node)
{
	auto parent = node->getParent();

	// if no parent, add node to graph
	if (parent == nullptr) {
		graph->addNode(node->clone());
	}
	else { // if parent, add to that parent
		parent->addChild(node);
	}
}

int fm::createDynamicLightId()
{
	// Global id, begins at 0
	static int global_id = GL_LIGHT0;

	// local ref to our id
	int id = global_id;

	// increment global id
	++global_id;
	
	// We need to figure out how to handle this later, but for now
	// just don't permit creation of more than 8 lights.
	assert(global_id >= GL_LIGHT0 || global_id <= GL_LIGHT7);

	return id;
}

// VECTOR 3 IMPLEMENTATION
// @author Paul Robertson
fm::Vector3::Vector3(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

fm::Vector3::Vector3() : Vector3(0, 0, 0) { }

fm::Vector3 fm::Vector3::copy() {
	Vector3 copy(
		this->x,
		this->y,
		this->z);
	return copy;
}

bool fm::Vector3::equals(const Vector3& v2, float epsilon) {
	return ((fabsf(this->x - v2.x) < epsilon) &&
		(fabsf(this->y - v2.y) < epsilon) &&
		(fabsf(this->z - v2.z) < epsilon));
}

bool fm::Vector3::equals(const Vector3& v2)
{
	return equals(v2, 0.00001f);
}

float fm::Vector3::length() {
	return (float)sqrt(this->lengthSquared());
}

float fm::Vector3::lengthSquared() {
	return (
		this->x*this->x +
		this->y*this->y +
		this->z*this->z
		);
}

void fm::Vector3::normalise() {
	float mag = this->length();
	if (mag) {
		float multiplier = 1.0f / mag;
		this->x *= multiplier;
		this->y *= multiplier;
		this->z *= multiplier;
	}
}

fm::Vector3 fm::Vector3::normalised()
{
	Vector3 norm(x, y, z);
	norm.normalise();
	return norm;
}

fm::Vector3 fm::Vector3::cross(const Vector3& v2) 
{
	Vector3 cross(
		(this->y * v2.z - this->z * v2.y),
		(this->z * v2.x - this->x * v2.z),
		(this->x * v2.y - this->y * v2.x)
	);
	return cross;
}

bool fm::Vector3::isZero()
{
	// might need to implement epsilon..
	return x == 0.0f && y == 0.0f && z == 0.0f;
}

void fm::Vector3::subtract(const Vector3& v1, float scale) 
{
	this->x -= (v1.x*scale);
	this->y -= (v1.y*scale);
	this->z -= (v1.z*scale);
}

void fm::Vector3::set(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

void fm::Vector3::setX(float x) {
	this->x = x;
}

void fm::Vector3::setY(float y) {
	this->y = y;
}

void fm::Vector3::setZ(float z) {
	this->z = z;
}

float fm::Vector3::getX() {
	return this->x;
}

float fm::Vector3::getY() {
	return this->y;
}

float fm::Vector3::getZ() {
	return this->z;
}

float fm::Vector3::dot(const Vector3& v2) {
	return (this->x*v2.x +
		this->y*v2.y +
		this->z*v2.z
		);
}

void fm::Vector3::scale(float scale) {
	this->x *= scale;
	this->y *= scale;
	this->z *= scale;
}

void fm::Vector3::add(const Vector3& v1, float scale) {
	this->x += (v1.x*scale);
	this->y += (v1.y*scale);
	this->z += (v1.z*scale);
}

fm::Vector3 fm::Vector3::operator+(const Vector3& v2) {
	return Vector3(this->x + v2.x, this->y + v2.y, this->z + v2.z);
}

fm::Vector3 fm::Vector3::operator-(const Vector3& v2) {
	return Vector3(this->x - v2.x, this->y - v2.y, this->z - v2.z);
}

fm::Vector3 fm::Vector3::operator/(const float & v)
{
	return Vector3(this->x / v, this->y / v, this->z / v);
}

fm::Vector3 fm::Vector3::operator+(const float & v)
{
	return Vector3(this->x + v, this->y + v, this->z + v);
}

fm::Vector3 fm::Vector3::operator-(const float & v)
{
	return Vector3(this->x - v, this->y - v, this->z - v);
}

fm::Vector3 fm::Vector3::operator*(const Vector3 & v2)
{
	return Vector3(x * v2.x, y * v2.y, z * v2.z);
}

fm::Vector3 fm::Vector3::operator*(const float& scalar)
{
	return Vector3(x * scalar, y * scalar, z * scalar);
}

fm::Vector3& fm::Vector3::operator+=(const Vector3& v2) {
	this->x += v2.x;
	this->y += v2.y;
	this->z += v2.z;
	return *this;
}

fm::Vector3& fm::Vector3::operator-=(const Vector3& v2) {
	this->x -= v2.x;
	this->y -= v2.y;
	this->z -= v2.z;
	return *this;
}

// INPUT IMPLEMENTATION
// @author Paul Robertson
fm::Input::Mouse::Mouse() 
{
}

void fm::Input::SetKeyDown(unsigned char key) {
	frameState.keys[key] = true;
}

void fm::Input::SetKeyUp(unsigned char key) {
	frameState.keys[key] = false;
}

bool fm::Input::isKeyDown(int key) {
	return frameState.keys[key];// && !lastFrameState.keys[key];
}

void fm::Input::setMouseX(int pos)
{
	frameState.mouse.x = pos;
}

void fm::Input::setMouseY(int pos)
{
	frameState.mouse.y = pos;
}

void fm::Input::setMousePos(int ix, int iy)
{
	frameState.mouse.x = ix;
	frameState.mouse.y = iy;
}

int fm::Input::getMouseX()
{
	return frameState.mouse.x;
}

int fm::Input::getMouseY()
{
	return frameState.mouse.y;
}

void fm::Input::setLeftMouseButton(bool b)
{
	frameState.mouse.left = b;
}

void fm::Input::setRightMouseButton(bool b)
{
	frameState.mouse.right = b;
}

bool fm::Input::isLeftMouseButtonPressed()
{
	return frameState.mouse.left;// && !lastFrameState.mouse.left;
}

bool fm::Input::isRightMouseButtonPressed()
{
	return frameState.mouse.right;// && !lastFrameState.mouse.right;
}

void fm::Input::setScrolling(float scrollAmount)
{
	frameState.mouse.scrollDirection = scrollAmount;
}

float fm::Input::scrollAmount()
{
	return frameState.mouse.scrollDirection;
}

// TEXTURE DATA IMPLEMENTATION
fm::TextureData::TextureData() : glTextureId(0) { }

// IMPLEMENTATION OF TEXTURE
fm::Texture::Texture() : data(nullptr) { }

fm::Texture::Texture(const std::string& fp) : Texture()
{
	data = AssetManager::global->getTextureData(fp);
}

// ASSET MANAGER IMPLEMENTATION
fm::AssetManager::AssetManager() : _loadedModelData(), _loadedTxData() { }

// Single instance of AssetManager
fm::AssetManager* fm::AssetManager::global = new AssetManager();

fm::AssetManager::~AssetManager()
{
	// delete all loaded model data
	for (auto modelData : _loadedModelData)
		delete modelData.second;

	_loadedModelData.clear();

	// delete all loaded texture data
	for (auto txData : _loadedTxData)
		delete txData.second;

	_loadedTxData.clear();
}

fm::TextureData * fm::AssetManager::getTextureData(const std::string & fp)
{
	// check if we have cached texture data
	TextureData* txrData = _loadedTxData[fp];

	// if we don't, load a texture, put it into the map
	if (txrData == nullptr) {
		txrData = new TextureData();

		// Assign filepath
		txrData->filepath = fp;

		// Load the texture
		txrData->glTextureId = SOIL_load_OGL_texture(fp.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

		// Put the texture into the map cache
		_loadedTxData[fp] = txrData;

		// Ensure texture loaded & was given a proper id
		assert(txrData->glTextureId != 0);
	}
	
	return txrData;
}

fm::ObjModel * fm::AssetManager::getObjModel(const std::string & fp)
{
	// check if we have this model loaded
	ObjModel* model = _loadedModelData[fp];

	if (model == nullptr) {
		// load the model, cache it
		model = loadObjModel(fp);

		_loadedModelData[fp] = model;
	}

	return model;
}

// TRANSFORM IMPLEMENTATION
fm::Transform::Transform() : position(0, 0, 0), scale(1, 1, 1), rotation(0, 0, 0), angle(0.0f) { }

fm::Transform::Transform(Vector3 & position, Vector3 & scale, Vector3 & rotation)
	: Transform(position, scale, rotation, 0.0f) { }

fm::Transform::Transform(Vector3 & position, Vector3 & scale, Vector3 & rotation, float angle)
	: position(position), scale(scale), rotation(rotation), angle(angle) { }

void fm::Transform::rotate(float amount)
{
	angle += amount;
}

void fm::Transform::move(Vector3 & v)
{
	move(v.x, v.y, v.z);
}

void fm::Transform::move(float x, float y)
{
	move(x, y, 0);
}

void fm::Transform::move(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
}

// CAMERA IMPLEMENTATION
fm::Camera::Camera(int w, int h) : _screenW(w), _screenH(h) 
{
	_rotation = Vector3(0, 0, 0);
	_position = Vector3(0, 0, 0);

	_dirty = false;

	// calculate our directional vectors
	calculateDirections();
}

void fm::Camera::pushController(CameraController * controller)
{
	_controlStack.push(controller);
	controller->start(this);
}

void fm::Camera::popController()
{
	// don't exit if there's no control stacks
	if (_controlStack.size() == 0) return;

	// get top, pop, delete controller
	CameraController* controller = _controlStack.top();
	_controlStack.pop();
	delete controller;
}

void fm::Camera::onScreenResize(int w, int h)
{
	_screenW = w;
	_screenH = h;

	if (h == 0)
		h = 1;

	float ratio = (float)w / (float)h;
	_fov = 45.0f;
	_nearPlane = 0.1f;
	_farPlane = 100.0f;

	// Use the projection matrix
	glMatrixMode(GL_PROJECTION);

	// Reset the matrix
	glLoadIdentity();

	// Set viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective
	gluPerspective(_fov, ratio, _nearPlane, _farPlane);

	// Back to model view
	glMatrixMode(GL_MODELVIEW);
}

void fm::Camera::pitch(float p)
{
	_rotation.x += p;
	_dirty = true;
}

void fm::Camera::yaw(float y)
{
	_rotation.y += y;
	_dirty = true;
}

void fm::Camera::setPosition(const Vector3& pos)
{
	_position = pos;
	_dirty = true;
}

void fm::Camera::setOrientation(const Vector3 & orientation)
{
	_rotation = orientation;
	_dirty = true;
}

void fm::Camera::move(Vector3 offset)
{
	_position += offset;
	_dirty = true;
}

void fm::Camera::calculateDirections() 
{
	static const auto pi = 3.1415;
	// rotate on x (pitch)
	float cosP = cosf(_rotation.x * pi / 180.0f);
	float sinP = sinf(_rotation.x * pi / 180.0f);
	// rotate on y (yaw)
	float cosY = cosf(_rotation.y * pi / 180.0f);
	float sinY = sinf(_rotation.y * pi / 180.0f);
	// rotate on z (roll)
	float sinR = sinf(_rotation.z * pi / 180.0f);
	float cosR = cosf(_rotation.z * pi / 180.0f);

	// calculate forward vector from our angle
	// and then apply our position so we know where we're looking
	_forward.x = sinY * cosP;
	_forward.y = sinP;
	_forward.z = cosP * -cosY;
	_forwardTarget = _forward + _position;

	// calculate up vector, unit vector
	_up.x = -cosY * sinR - sinY * sinP * cosR;
	_up.y = cosP * cosR;
	_up.z = -sinY * sinR - sinP * cosR * -cosY;

	// calculate right, which is a cross product between forward and up
	_right = _forward.cross(_up);
}

void fm::Camera::update(float dt) 
{
	// if there have been no changes, don't run
	if (_dirty) {
		calculateDirections();
		_dirty = false;
	}

	// if we have a controller in stack, get top and update it.
	if (!_controlStack.empty()) {
		_controlStack.top()->update(this, dt);
	}
}

void fm::Camera::view() 
{
	// Reset transformations
	glLoadIdentity();
	
	// Set the camera
	gluLookAt(
		// position in world space
		_position.x, _position.y, _position.z, 
		// lookAt - what the camera is looking at
		_forwardTarget.x, _forwardTarget.y, _forwardTarget.z,
		// Up - Up direction relative to the camera.
		_up.x, _up.y, _up.z
	);
}

void fm::Camera::reset()
{
	_position = Vector3();
	_rotation = Vector3();

	_dirty = true;
}

int fm::Camera::getCentreX()
{
	return _screenW / 2;
}

int fm::Camera::getCentreY()
{
	return _screenH / 2;
}

int fm::Camera::getScreenWidth()
{
	return _screenW;
}

int fm::Camera::getScreenHeight()
{
	return _screenH;
}

float fm::Camera::getFov()
{
	return _fov;
}

float fm::Camera::getNearPlane()
{
	return _nearPlane;
}

float fm::Camera::getFarPlane()
{
	return _farPlane;
}

fm::Vector3 fm::Camera::up()
{
	return _up;
}

fm::Vector3 fm::Camera::down()
{
	return Vector3(-_up.x, -_up.y, -_up.z);
}

fm::Vector3 fm::Camera::forward()
{
	return _forward;
}

fm::Vector3 fm::Camera::back()
{
	return Vector3(-_forward.x, -_forward.y, -_forward.z);
}

fm::Vector3 fm::Camera::right()
{
	return _right;
}

const fm::Vector3& fm::Camera::getPosition()
{
	return _position;
}

const fm::Vector3& fm::Camera::getRotation()
{
	return _rotation;
}

fm::Vector3 fm::Camera::left()
{
	return Vector3(-_right.x, -_right.y, -_right.z);
}

// COLOR IMPLEMENTATION
fm::Color::Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) { }

fm::Color::Color(float r, float g, float b) : Color(r, g, b, 1) { }

fm::Color::Color() : Color(1, 1, 1, 1) { }

// MATERIAL IMPLEMENTATION
fm::Material::Material() : diffuseColor(), ambientColor(), specularColor(), 
	doubleSided(false), specularEnabled(false), shininess(16), texture(nullptr) { }

fm::Material::~Material()
{
	if (texture != nullptr) {
		delete texture;
		texture = nullptr;
	}
}

// TRI IMPLEMENTATION
fm::Tri::Tri() { }

fm::Tri::Tri(Vector3 v1, Vector3 v2, Vector3 v3) : v1(v1), v2(v2), v3(v3) { }

// SCENE NODE GRAPH IMPLEMENTATION
fm::SceneNodeGraph::~SceneNodeGraph()
{
	// delete all known nodes
	for (auto node : _nodes)
		delete node;
}

// Sorts nodes by comparing categories
bool sortNodeByCategory(fm::SceneNode* a, fm::SceneNode* b) 
{
	return a->category() < b->category();
}

fm::SceneNode* fm::SceneNodeGraph::addNode(SceneNode * node)
{
	_nodes.push_back(node);

	std::sort(_nodes.begin(), _nodes.end(), sortNodeByCategory);

	return node;
}

void fm::SceneNodeGraph::render()
{
	// render all the known nodes
	for (auto node : _nodes) {
		if (!node->enabled) continue;
		node->render();
	}
}

int fm::SceneNodeGraph::nodeCount()
{
	int count = _nodes.size();

	for (auto node : _nodes) {
		count += node->childCount();
	}

	return count;
}

std::vector<fm::SceneNode*>& fm::SceneNodeGraph::getNodes()
{
	return _nodes;
}

void fm::SceneNodeGraph::removeNode(SceneNode * node)
{
	if (!removeNodeFromVector(node, _nodes)) {
		// TODO handle fall through
		// this means that no node was erased..
	}
}

// SCENE NODE IMPLEMENTATION
fm::SceneNode::SceneNode()
{
	static unsigned int globalUID = 0;
	++globalUID;
	_uid = globalUID;

	name = "Scene Node";
	_parent = nullptr;
	enabled = true;
	nodeCategory = DEFAULT_NODE_CATEGORY;
}

fm::SceneNode::SceneNode(SceneNode * node) : SceneNode()
{
	name = node->name;
	_parent = node->_parent;
	enabled = node->enabled;
	nodeCategory = node->nodeCategory;

	// copy child nodes as well, if there are any
	if (!node->childNodes.empty()) {
		for (auto copyNode : node->childNodes) {
			this->childNodes.push_back(copyNode->clone());
		}
	}
}

fm::SceneNode::~SceneNode()
{
	// delete all children
	for (auto child : childNodes)
		delete child;

	childNodes.clear();
}

void fm::SceneNode::render()
{
	for (auto child : childNodes) {
		if (!child->enabled) continue;

		child->render();
	}
}

void fm::SceneNode::addChild(SceneNode * child)
{
	// Ensure that the new child doesn't have a parent already
	assert(child->_parent == nullptr);
	// Assign child parent to this node
	child->_parent = this;
	// Put child into the collection
	childNodes.push_back(child);
}

fm::SceneNode* fm::SceneNode::removeChild(SceneNode * child)
{
	// try to remove node from the vector, if fail, return nullptr
	if (!removeNodeFromVector(child, childNodes)) {
		return nullptr;
	}
	else
		return child;
}

int fm::SceneNode::category()
{
	return nodeCategory;
}

fm::SceneNode * fm::SceneNode::getParent()
{
	return _parent;
}

int fm::SceneNode::getUniqueId()
{
	return _uid;
}

int fm::SceneNode::childCount()
{
	int size = childNodes.size();
	for (auto node : childNodes)
		size += node->childCount();

	return size;
}

// SHAPE NODE IMPLEMENTATION
fm::ShapeNode::ShapeNode(Color color) : SceneNode()
{
	material.ambientColor = color;
	name = "Unnamed Shape Node";
}

fm::ShapeNode::ShapeNode(ShapeNode * node) : SceneNode(node)
{
	material = node->material;
}

fm::SceneNode * fm::ShapeNode::clone()
{
	return new ShapeNode(this);
}

// CUBE NODE IMPLEMENTATION
fm::CubeNode::CubeNode(Color color) : ShapeNode(color) 
{
	name = "Cube Node";
}

fm::CubeNode::CubeNode(const std::string & texture) : CubeNode()
{
	material.texture = new Texture(texture);
}

fm::CubeNode::CubeNode() : CubeNode(Color(1, 1, 1, 1)) { }

fm::CubeNode::CubeNode(CubeNode * node) : ShapeNode(node) { }



fm::SceneNode * fm::CubeNode::clone()
{
	return new CubeNode(this);
}

void fm::CubeNode::render()
{
	glPushMatrix();
	applyTransform(transform);
	applyMaterial(material);

	bool txrApplied = applyTexture(material);

	// Cube vertices, render order is: FRONT -> RIGHT -> BOTTOM -> LEFT -> TOP -> BACK
	static const GLfloat vertices[] = {
		// front
		-0.5f, -0.5f, 0.5f,		0.5f, -0.5f, 0.5f,		0.5f, 0.5f, 0.5f,	-0.5f, 0.5f, 0.5f,
		// right
		0.5f, -0.5f, -0.5f,		0.5f, 0.5f, -0.5f,		0.5f, 0.5f, 0.5f,	0.5f, -0.5f, 0.5f,
		// bottom
		0.5f, -0.5f, 0.5f,		0.5f, -0.5f, -0.5f,		-0.5f, -0.5f, -0.5f,  -0.5f, -0.5f, 0.5f,
		// left
		-0.5f, -0.5f, 0.5f,		-0.5f, -0.5f, -0.5f,	-0.5f, 0.5f, -0.5f,		-0.5f, 0.5f, 0.5f,
		// top
		-0.5f, 0.5f, 0.5f,		0.5f, 0.5f, 0.5f,		0.5f, 0.5f, -0.5f,		-0.5f, 0.5f, -0.5f,
		// back
		-0.5f, -0.5f, -0.5f,	0.5f, -0.5f, -0.5f,		0.5f, 0.5f, -0.5f,		-0.5f, 0.5f, -0.5f
	};

	// Cube normals
	static const GLfloat normals[] = {
		//front
		0.0f, 0.0f, 1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
		// right
		1.0f, 0.0f, 0.0f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f, 0.0f,
		// bottom
		0.0f, -1.0f, 0.0f,		0.0f, -1.0f, 0.0f,		0.0f, -1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		// left
		-1.0f, 0.0f, 0.0f,		-1.0f, 0.0f, 0.0f,		-1.0f, 0.0f, 0.0f,		-1.0f, 0.0f, 0.0f,
		// top
		0.0f, 1.0f, 0.0f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f, 0.0f,
		// back
		0.0f, 0.0f, -1.0f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f, -1.0f
	};

	// Cube UVs
	static const GLfloat uvs[] = {
		// front
		0.0f, 0.5f,			0.25f, 0.5f,		0.25f, 0.25f,		0.0f, 0.25f,
		// right
		0.25f, 0.75f,		0.5f, 0.75f,		0.5f, 0.5f,			0.25f, 0.5f,
		// bottom
		0.25f, 0.5f,		0.5f, 0.5f,			0.5f, 0.25f,		0.25f, 0.25f,
		// left
		0.25f, 0.25f,		0.5f, 0.25f,		0.5f, 0.0f,			0.25f, 0.0f,
		// top
		0.75f, 0.5f,		1.0f, 0.5f,			1.0f, 0.25f,		0.75f, 0.25f,
		// back
		0.5f, 0.5f,			0.75f, 0.5f,		0.75f, 0.25f,		0.5f, 0.25f,
	};

	// enable and pass in our arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glNormalPointer(GL_FLOAT, 0, normals);

	// if we're using the texture, use the tex coord array
	if (txrApplied) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, uvs);
	}

	glDrawArrays(GL_QUADS, 0, 24);

	// disable our client states
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	if (txrApplied) {
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	// Draw children
	SceneNode::render();

	glPopMatrix();
}

// SPHERE NODE IMPLEMENTATION
fm::SphereNode::SphereNode(Color color) : ShapeNode(color) 
{
	name = "Sphere Node";
	_slices = 20;
	_stacks = 20;
}

fm::SphereNode::SphereNode() : SphereNode(Color(1, 1, 1, 1)) { }

fm::SphereNode::SphereNode(SphereNode * node) : ShapeNode(node)
{
	_slices = node->_slices;
	_stacks = node->_stacks;
}

fm::SceneNode * fm::SphereNode::clone()
{
	return new SphereNode(this);
}

int & fm::SphereNode::getSlices()
{
	return _slices;
}

int & fm::SphereNode::getStacks()
{
	return _stacks;
}

void fm::SphereNode::render()
{
	glPushMatrix();
	applyTransform(transform);
	applyMaterial(material);

	gluSphere(gluNewQuadric(), 1, _slices, _stacks);

	SceneNode::render();
	glPopMatrix();
}

// PLANE NODE IMPLEMENTATION
fm::PlaneNode::PlaneNode(Color color, int quadSize, int width, int height) : ShapeNode(color) 
{
	buildQuads(quadSize, width, height);
	name = "Plane Node";
}

fm::PlaneNode::PlaneNode() : PlaneNode(Color(1, 1, 1, 1), 4, 1, 1) { }

fm::PlaneNode::PlaneNode(PlaneNode * node) : ShapeNode(node)
{
	buildQuads(node->_quadSize, node->_width, node->_height);
}

fm::SceneNode * fm::PlaneNode::clone()
{
	return new PlaneNode(this);
}

void fm::PlaneNode::render()
{
	glPushMatrix();
	applyTransform(transform);
	applyMaterial(material);

	bool texApplied = applyTexture(material);

	// On a plane, all normals face up.
	static const Vector3 planeNormal = Vector3(0, 1, 0);

	glBegin(GL_TRIANGLES);

	// now write every triangle we built with buildQuads()
	for (int i = 0; i < _tris.size(); ++i) {
		Tri& tri = _tris[i];
		Tri& uv = _uvs[i];

		// draw vertex1
		glNormal3f(planeNormal.x, planeNormal.y, planeNormal.z);
		if (texApplied)
			glTexCoord2f(uv.v1.x, uv.v1.y);
		glVertex3f(tri.v1.x, tri.v1.y, tri.v1.z);

		// draw vertex2
		glNormal3f(planeNormal.x, planeNormal.y, planeNormal.z);
		if (texApplied)
			glTexCoord2f(uv.v2.x, uv.v2.y);
		glVertex3f(tri.v2.x, tri.v2.y, tri.v2.z);

		// draw vertex3
		glNormal3f(planeNormal.x, planeNormal.y, planeNormal.z);
		if (texApplied)
			glTexCoord2f(uv.v3.x, uv.v3.y);
		glVertex3f(tri.v3.x, tri.v3.y, tri.v3.z);
	}
	
	glEnd();

	SceneNode::render();

	glPopMatrix();
}

int fm::PlaneNode::quadLength()
{
	return _quadSize;
}

int fm::PlaneNode::width()
{
	return _width;
}

int fm::PlaneNode::height()
{
	return _height;
}

void fm::PlaneNode::buildQuads(int size, int width, int height)
{
	_tris.clear();
	_uvs.clear();

	_quadSize = size;
	_width = width;
	_height = height;

	float sizef = (float)size;
	float x_offset = (sizef * (float)width) / 2.f;
	float y_offset = (sizef * (float)height) / 2.f;

	float uv_x_increment = 1.0f / width;
	float uv_y_increment = 1.0f / height;

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			// we're thinking in 2d, imagining a 2d surface, instead we need to think in topdown..
			float x_pos = (float)(x * size) - x_offset;
			float y_pos = (float)(y * size) - y_offset;

			// so what we would see topdown as (x, y) is actually (x, z)
			Vector3 v1 = Vector3(x_pos,				0,		y_pos);
			Vector3 v2 = Vector3(x_pos + sizef,		0,		y_pos);
			Vector3 v3 = Vector3(x_pos + sizef,		0,		y_pos + sizef);
			Vector3 v4 = Vector3(x_pos,				0,		y_pos + sizef);

			// Tri 1 is v1/v3/v4, Tri 2 is v1/v2/v3
			_tris.push_back( Tri( v1, v3, v4) );
			_tris.push_back( Tri( v1, v2, v3 ) );

			// Now we want to figure out the UVs..
			float xuv = (float)x * uv_x_increment;
			float yuv = (float)y * uv_y_increment;

			// organised as uv origin top-left coordinates, rather than matching the vertices
			Vector3 uv1 = Vector3(xuv, yuv, 0); // top left
			Vector3 uv2 = Vector3(xuv + uv_x_increment, yuv, 0); // top right
			Vector3 uv3 = Vector3(xuv, yuv + uv_y_increment, 0); // bottom left
			Vector3 uv4 = Vector3(xuv + uv_x_increment, yuv + uv_y_increment, 0); // bottom right

			_uvs.push_back(Tri(uv3, uv2, uv1));
			_uvs.push_back(Tri(uv3, uv4, uv2));
		}
	}
}

// LIGHT NODE IMPLEMENTATION
fm::LightNode::LightNode(Color color) : color(color) 
{
	name = "Light Node";
	lightId = createDynamicLightId();
	nodeCategory = LIGHT_CATEGORY;
}

fm::LightNode::LightNode(LightNode * node) : SceneNode(node)
{
	lightId = createDynamicLightId();
	color = node->color;
}

// AMBIENT LIGHT NODE IMPLEMENTATION
fm::AmbientLightNode::AmbientLightNode(Color color) : LightNode(color) 
{
	name = "Ambient Light Node";
}

fm::AmbientLightNode::AmbientLightNode(Color lightColor, Color diffuseColor) : AmbientLightNode(lightColor) 
{
	diffuse = diffuseColor;
}

fm::AmbientLightNode::AmbientLightNode() : AmbientLightNode(Color(1, 1, 1, 1), Color(1, 1, 1, 1)) { }

fm::AmbientLightNode::AmbientLightNode(AmbientLightNode * node) : LightNode(node)
{
	diffuse = node->diffuse;
}

void fm::AmbientLightNode::render()
{
	float light_ambient[] = { color.r, color.g, color.b, color.a };
	float light_diffuse[] = { diffuse.r, diffuse.g, diffuse.b, diffuse.a };
	float light_position[] = { transform.position.x, transform.position.y, transform.position.z, 1.0f };

	glLightfv(lightId, GL_AMBIENT, light_ambient);
	glLightfv(lightId, GL_DIFFUSE, light_diffuse);
	glLightfv(lightId, GL_POSITION, light_position);
	glEnable(lightId);
}

fm::SceneNode * fm::AmbientLightNode::clone()
{
	return new AmbientLightNode(this);
}

// DIRECTIONAL LIGHT NODE IMPLEMENTATION
fm::DirectionalLightNode::DirectionalLightNode(Color color) : LightNode(color) 
{
	name = "Directional Light Node";
}

fm::DirectionalLightNode::DirectionalLightNode() : DirectionalLightNode(Color(1, 1, 1, 1)) { }

fm::DirectionalLightNode::DirectionalLightNode(DirectionalLightNode * node) : LightNode(node) { }

void fm::DirectionalLightNode::render()
{
	// w axis is 0, indicating directional light
	float light_position[] = { transform.position.x, transform.position.y, transform.position.z, 0.0f };
	float light_diffuse[] = { color.r, color.g, color.b, color.a };
	
	glLightfv(lightId, GL_DIFFUSE, light_diffuse);
	glLightfv(lightId, GL_POSITION, light_position);
	glEnable(lightId);
}

fm::SceneNode * fm::DirectionalLightNode::clone()
{
	return new DirectionalLightNode(this);
}

// SPOT LIGHT NODE IMPLEMENTATION
fm::SpotLightNode::SpotLightNode(Color lightColor, Color diffuseColor, Vector3 dir)
	: LightNode(lightColor)
{
	diffuse = diffuseColor;
	direction = dir;
	cutoff = 25.0f;
	exponent = 50.0f;

	name = "Spot Light Node";
}

fm::SpotLightNode::SpotLightNode()
	: SpotLightNode(Color(1, 1, 1, 1), Color(1, 1, 1, 1), Vector3(1, 1, 1)) { }

fm::SpotLightNode::SpotLightNode(SpotLightNode * node) : LightNode(node)
{
	cutoff = node->cutoff;
	exponent = node->exponent;
	direction = node->direction;
	diffuse = node->diffuse;
}

void fm::SpotLightNode::render()
{
	GLfloat light_ambient[] = { color.r, color.g, color.b, color.a };
	GLfloat light_diffuse[] = { diffuse.r, diffuse.g, diffuse.b, diffuse.a };
	GLfloat light_position[] = { transform.position.x, transform.position.y, transform.position.z, 1.0f };
	GLfloat spot_direction[] = { direction.x, direction.y, direction.z };

	glLightfv(lightId, GL_AMBIENT, light_ambient);
	glLightfv(lightId, GL_DIFFUSE, light_diffuse);
	glLightfv(lightId, GL_POSITION, light_position);
	glLightf(lightId, GL_SPOT_CUTOFF, cutoff);
	glLightfv(lightId, GL_SPOT_DIRECTION, spot_direction);
	glLightf(lightId, GL_SPOT_EXPONENT, exponent);
	glEnable(lightId);
}

fm::SceneNode * fm::SpotLightNode::clone()
{
	return new SpotLightNode(this);
}

// MESH NODE IMPLEMENTATION
fm::MeshNode::MeshNode() : model(nullptr), material() 
{ 
	name = "Mesh Node";
}

fm::MeshNode::MeshNode(const std::string& modelPath) : MeshNode() 
{
	model = AssetManager::global->getObjModel(modelPath);
}

fm::MeshNode::MeshNode(MeshNode * node) : SceneNode(node)
{
	material = node->material;
	model = node->model;
}

void fm::MeshNode::render()
{
	glPushMatrix();
	applyTransform(transform);
	applyMaterial(material);

	// if the model hasn't been loaded yet, nothing to render.
	if (model != nullptr) {
		// Check if we're using a texture, if so, bind it!
		Texture* texture = material.texture;
		bool usingTexture = applyTexture(material);

		glBegin(GL_TRIANGLES);

		// for every face of the model..
		for (auto& face : model->polyFaces) {
			// get the indexes to the vertex/tex/normal
			for (auto& index : face.indices) {
				// indexes in obj models start at 1, not 0, so remove 1
				auto& vertex = model->vertices[index.vertexIndex - 1];
				auto& normal = model->vertexNormals[index.normalIndex - 1];

				// use the normal
				glNormal3f(normal.x, normal.y, normal.z);

				// if we're using the texture..
				if (usingTexture) {
					// use the texture coords for the texture we've bound
					auto& texcoord = model->textureCoords[index.texCoordIndex - 1];
					glTexCoord2f(texcoord.x, texcoord.y);
				}
				
				// use the vertex
				glVertex3f(vertex.x, vertex.y, vertex.z);
			}
		}

		glEnd();
	}
	
	SceneNode::render();

	glPopMatrix();
}

fm::SceneNode * fm::MeshNode::clone()
{
	return new MeshNode(this);
}

// IMPLEMENTATION OF CYLINDER NODE
fm::CylinderNode::CylinderNode() : ShapeNode(Color(1, 1, 1, 1))
{
	name = "Cylinder Node";

	// build the cylinder vertex data
	build(20);
}

fm::CylinderNode::CylinderNode(CylinderNode * node) : ShapeNode(node)
{
	// copy vertices
	this->_vertices = node->_vertices;
}

int fm::CylinderNode::numSegments()
{
	return _numSegments;
}

void fm::CylinderNode::build(int segments)
{
	// ensure that segments is not a negative count
	assert(segments > 0);

	_vertices.clear();
	_uvs.clear();
	_normals.clear();

	_numSegments = segments;

	float theta = 0.0f;
	float delta = 0.0f;

	float theta_increment = (2.0 * 3.1415) / segments;
	float delta_increment = 3.1415 / segments;
	float uv_increment = 1.0 / segments;

	// loop long->lat, figure out the vertex position, normals + tex coords
	for (int longSegment = 0; longSegment < segments; longSegment++) {

		// reset theta to 0.0
		theta = 0.0f;

		for (int latSegment = 0; latSegment < segments; latSegment++) {

			// calculate UVs
			float uvx = uv_increment * longSegment;
			float uvy = uv_increment * latSegment;
			Vector3 uv1 = Vector3(uvx, uvy, 0); // top left
			Vector3 uv2 = Vector3(uvx + uv_increment, uvy, 0); // top right
			Vector3 uv3 = Vector3(uvx + uv_increment, uvy + uv_increment, 0); // bot right
			Vector3 uv4 = Vector3(uvx, uvy + uv_increment, 0); // bot left

			// figure out vertex1
			pushVertUv(
				cosf(theta) * sinf(delta),
				cosf(delta), 
				sinf(theta) * sinf(delta),
				uv4.x, uv4.y
			);

			// figure out vertex2
			pushVertUv(
				cosf(theta + theta_increment) * sinf(delta),
				cosf(delta),
				sinf(theta + theta_increment) * sinf(delta),
				uv3.x, uv3.y
			);

			// figure out vertex3
			pushVertUv(
				cosf(theta + theta_increment) * sinf(delta + delta_increment),
				cosf(delta + delta_increment),
				sinf(theta + theta_increment) * sinf(delta + delta_increment),
				uv2.x, uv2.y
			);

			// figure out vertex4
			pushVertUv(
				cosf(theta) * sinf(delta + delta_increment),
				cosf(delta + delta_increment),
				sinf(theta) * sinf(delta + delta_increment),
				uv1.x, uv1.y
			);

			// increment theta
			theta += theta_increment;
		}

		// increment delta
		delta += delta_increment;
	}

	// cheap hack to reduce sphere to 1 unit
	for (int i = 0; i < _vertices.size(); i++) {
		float& vertex = _vertices[i];
		vertex /= 2.0;

		float& normal = _normals[i];
		normal /= 2.0;
	}
}

void fm::CylinderNode::pushVertUv(float x, float y, float z, float u, float v)
{
	_vertices.push_back(x);
	_vertices.push_back(y);
	_vertices.push_back(z);

	// In a unit circle, where r = 1, the normal direction
	// is the same as the vertex position.
	_normals.push_back(x);
	_normals.push_back(y);
	_normals.push_back(z);

	_uvs.push_back(u);
	_uvs.push_back(v);
}

void fm::CylinderNode::render()
{
	glPushMatrix();
	applyTransform(transform);
	applyMaterial(material);

	bool texApplied = applyTexture(material);

	// enable array states, pass in all
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, _vertices.data());
	glNormalPointer(GL_FLOAT, 0, _normals.data());

	if (texApplied) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, _uvs.data());
	}
	
	// renders vertices, normals, uvs..
	glDrawArrays(GL_QUADS, 0, _vertices.size() / 3);
	
	// disable array states
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	if (texApplied)
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glPopMatrix();
}

fm::SceneNode * fm::CylinderNode::clone()
{
	return new CylinderNode(this);
}

