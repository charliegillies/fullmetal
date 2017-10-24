#include "fullmetal.h"
#include "fullmetal-3d.h"
#include "glut.h"

#include <math.h>
#include <cassert>

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

void fm::normalVertex(const Vector3 & normal, float x, float y, float z)
{
	glNormal3f(normal.x, normal.y, normal.z);
	glVertex3f(x, y, z);
}

bool fm::removeNodeFromVector(SceneNode * node, std::vector<SceneNode*>& _nodes)
{
	for (auto i = _nodes.begin(); i != _nodes.end(); ++i) {
		auto n = *i;

		// if found node, erase..
		if (n == node) {
			_nodes.erase(i);
			return true;
		}
	}

	return false;
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

void fm::moveNodeUpHierarchy(SceneNode * node, SceneNodeGraph * graph)
{
}

void fm::moveNodeDownHierarchy(SceneNode * node, SceneNodeGraph * graph)
{
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

fm::Vector3 fm::Vector3::cross(const Vector3& v2) {
	Vector3 cross(
		(this->y * v2.z - this->z * v2.y),
		(this->z * v2.x - this->x * v2.z),
		(this->x * v2.y - this->y * v2.x)
	);
	return cross;
}

void fm::Vector3::subtract(const Vector3& v1, float scale) {
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

// IMPLEMENTATION OF TEXTURE DATA
fm::TextureData* fm::loadTextureData(std::string fp)
{
	TextureData* txrData = new TextureData();

	// Assign filepath
	txrData->filepath = fp;

	// Load the texture
	txrData->glTextureId = SOIL_load_OGL_texture(
		fp.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

	// Ensure texture loaded & was given a proper id
	assert(txrData->glTextureId != 0);

	return txrData;
}

// IMPLEMENTATION OF TEXTURE
fm::Texture::Texture() : data(nullptr) { }

fm::Texture::Texture(std::string fp) : Texture()
{
	data = loadTextureData(fp);
}

fm::Texture::~Texture()
{
	if (data != nullptr) {
		delete data;
	}
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
fm::Camera::Camera(int w, int h) : _transform(), _screenW(w), _screenH(h) {
	_pitch = 0.0f;
	_roll = 0.0f;
	_yaw = 0.0f;

	_dirty = false;

	// calculate our directional vectors
	calculateDirections();
}

void fm::Camera::onScreenResize(int w, int h)
{
	_screenW = w;
	_screenH = h;
}

void fm::Camera::pitch(float p)
{
	_pitch += p;
	_dirty = true;
}

void fm::Camera::yaw(float y)
{
	_yaw += y;
	_dirty = true;
}

void fm::Camera::setPosition(Vector3 pos)
{
	_transform.position = pos;
	_dirty = true;
}

void fm::Camera::move(Vector3 offset)
{
	_transform.position += offset;
	_dirty = true;
}

void fm::Camera::calculateDirections() 
{
	// Calculate cos & sin values for each axis rot (yaw, pitch, roll)
	static const auto pi = 3.1415;
	float cosR = cosf(_yaw * pi / 180.0f);
	float cosP = cosf(_pitch * pi / 180.0f);
	float cosY = cosf(_roll * pi / 180.0f);
	float sinY = sinf(_yaw * pi / 180.0f);
	float sinP = sinf(_pitch * pi / 180.0f);
	float sinR = sinf(_roll * pi / 180.0f);

	// calculate forward vector from our angle
	// and then apply our position so we know where we're looking
	_forward.x = sinY * cosP;
	_forward.y = sinP;
	_forward.z = cosP * -cosY;
	_forwardTarget = _forward + _transform.position;

	// calculate up vector, unit vector
	_up.x = -cosY * sinR - sinY * sinP * cosR;
	_up.y = cosP * cosR;
	_up.z = -sinY * sinR - sinP * cosR * -cosY;

	// calculate right, which is a cross product between forward and up
	_right = _forward.cross(_up);
}

void fm::Camera::update() 
{
	// if there have been no changes, don't run
	if (!_dirty) {
		return;
	}

	calculateDirections();
	_dirty = false;
}

void fm::Camera::view() 
{
	// Reset transformations
	glLoadIdentity();
	
	auto& position = _transform.position;

	// Set the camera
	gluLookAt(
		// position in world space
		position.x, position.y, position.z, 
		// lookAt - what the camera is looking at
		_forwardTarget.x, _forwardTarget.y, _forwardTarget.z,
		// Up - Up direction relative to the camera.
		_up.x, _up.y, _up.z
	);
}

int fm::Camera::getCentreX()
{
	return _screenW / 2;
}

int fm::Camera::getCentreY()
{
	return _screenH / 2;
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

fm::SceneNode* fm::SceneNodeGraph::addNode(SceneNode * node)
{
	_nodes.push_back(node);
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
fm::ShapeNode::ShapeNode(Color color) {
	material.ambientColor = color;
	name = "Shape Node";
}

// CUBE NODE IMPLEMENTATION
fm::CubeNode::CubeNode(Color color) : ShapeNode(color) {
	name = "Cube Node";
}

fm::CubeNode::CubeNode() : CubeNode(Color(1, 1, 1, 1)) { }

void fm::CubeNode::render()
{
	glPushMatrix();
	applyTransform(transform);
	applyMaterial(material);

	glBegin(GL_QUADS);

		//Front
		const Vector3 frontNormal(0.0f, 0.0f, 1.0f);
		normalVertex(frontNormal, -1.0f, -1.0f, 1.0f);
		normalVertex(frontNormal, 1.0f, -1.0f, 1.0f);
		normalVertex(frontNormal, 1.0f, 1.0f, 1.0f);
		normalVertex(frontNormal, -1.0f, 1.0f, 1.0f);

		// Right
		//fmColor(Color(0.5f, 0.0f, 0.5f, 1.0f));
		const Vector3 rightNormal(1.0f, 0.0f, 0.0f);
		normalVertex(rightNormal, 1.0f, -1.0f, -1.0f);
		normalVertex(rightNormal, 1.0f, 1.0f, -1.0f);
		normalVertex(rightNormal, 1.0f, 1.0f, 1.0f);
		normalVertex(rightNormal, 1.0f, -1.0f, 1.0f);

		// Bottom
		//fmColor(Color(0.0f, 0.5f, 0.5f, 1.0f));
		const Vector3 bottomNormal(0.0f, -1.0f, 0.0f);
		normalVertex(bottomNormal, 1.0f, -1.0f, 1.0f);
		normalVertex(bottomNormal, 1.0f, -1.0f, -1.0f);
		normalVertex(bottomNormal, -1.0f, -1.0f, -1.0f);
		normalVertex(bottomNormal, -1.0f, -1.0f, 1.0f);

		// Left
		//fmColor(Color(0.0f, 0, 1, 1.0f));
		const Vector3 leftNormal(-1.0f, 0.0f, 0.0f);
		normalVertex(leftNormal, -1.0f, -1.0f, 1.0f);
		normalVertex(leftNormal, -1.0f, -1.0f, -1.0f);
		normalVertex(leftNormal, -1.0f, 1.0f, -1.0f);
		normalVertex(leftNormal, -1.0f, 1.0f, 1.0f);

		// Top
		//fmColor(Color(0.0f, 1, 0, 1.0f));
		const Vector3 topNormal(0, 1, 0);
		normalVertex(topNormal, -1.0f, 1.0f, 1.0f);
		normalVertex(topNormal, 1.0f, 1.0f, 1.0f);
		normalVertex(topNormal, 1.0f, 1.0f, -1.0f);
		normalVertex(topNormal, -1.0f, 1.0f, -1.0f);

		// Back
		//fmColor(Color(1.0f, 0, 0, 1.0f));
		const Vector3 backNormal(0, 0, -1);
		normalVertex(backNormal, -1.0f, -1.0f, -1.0f);
		normalVertex(backNormal, 1.0f, -1.0f, -1.0f);
		normalVertex(backNormal, 1.0f, 1.0f, -1.0f);
		normalVertex(backNormal, -1.0f, 1.0f, -1.0f);

	glEnd();

	// Draw children
	SceneNode::render();

	glPopMatrix();
}

// SPHERE NODE IMPLEMENTATION
fm::SphereNode::SphereNode(Color color) : ShapeNode(color) {
	name = "Sphere Node";
	_slices = 20;
	_stacks = 20;
}

fm::SphereNode::SphereNode() : SphereNode(Color(1, 1, 1, 1)) { }

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
fm::PlaneNode::PlaneNode(Color color, int quadSize, int width, int height) : ShapeNode(color) {
	buildQuads(quadSize, width, height);
	name = "Plane Node";
}

fm::PlaneNode::PlaneNode() : PlaneNode(Color(1, 1, 1, 1), 4, 1, 1) { }

void fm::PlaneNode::render()
{
	glPushMatrix();
	applyTransform(transform);
	applyMaterial(material);

	glBegin(GL_TRIANGLES);

	// On a plane, all normals face up.
	static const Vector3 planeNormal = Vector3(0, 1, 0);

	// now write every triangle we built with buildQuads()
	for (int i = 0; i < _tris.size(); ++i) {
		Tri& tri = _tris[i];
		normalVertex(planeNormal, tri.v1.x, tri.v1.y, tri.v1.z);
		normalVertex(planeNormal, tri.v2.x, tri.v2.y, tri.v2.z);
		normalVertex(planeNormal, tri.v3.x, tri.v3.y, tri.v3.z);
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
	_quadSize = size;
	_width = width;
	_height = height;

	_tris.clear();

	float x_offset = (size * width) / 2;
	float y_offset = (size * height) / 2;

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {

			float x_pos = (float)x - x_offset;
			float y_pos = (float)y - y_offset;

			Vector3 v1 = Vector3(x_pos, 0, y_pos);
			Vector3 v2 = Vector3(x_pos + (float)size, 0, y_pos);
			Vector3 v3 = Vector3(x_pos + (float)size, 0, y_pos + (float)size);
			Vector3 v4 = Vector3(x_pos, 0, y_pos + (float)size);

			Tri tri1 = Tri(v1, v3, v4); //v1 v2 v4 make up the top left part
			Tri tri2 = Tri(v1, v2, v3); //v1 v2 v3 make up the bottom right part

			_tris.push_back(tri1);
			_tris.push_back(tri2);
		}
	}
}

// LIGHT NODE IMPLEMENTATION
fm::LightNode::LightNode(Color color) : color(color) {
	name = "Light Node";
	lightId = createDynamicLightId();
}

// AMBIENT LIGHT NODE IMPLEMENTATION
fm::AmbientLightNode::AmbientLightNode(Color color) : LightNode(color) {
	name = "Ambient Light Node";
}

fm::AmbientLightNode::AmbientLightNode(Color lightColor, Color diffuseColor) : AmbientLightNode(lightColor) {
	diffuse = diffuseColor;
}

fm::AmbientLightNode::AmbientLightNode()
	: AmbientLightNode(Color(1, 1, 1, 1), Color(1, 1, 1, 1)) { }

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

// DIRECTIONAL LIGHT NODE IMPLEMENTATION
fm::DirectionalLightNode::DirectionalLightNode(Color color) : LightNode(color) {
	name = "Directional Light Node";
}

fm::DirectionalLightNode::DirectionalLightNode()
	: DirectionalLightNode(Color(1, 1, 1, 1)) { }

void fm::DirectionalLightNode::render()
{
	// w axis is 0, indicating directional light
	float light_position[] = { transform.position.x, transform.position.y, transform.position.z, 0.0f };
	float light_diffuse[] = { color.r, color.g, color.b, color.a };
	
	glLightfv(lightId, GL_DIFFUSE, light_diffuse);
	glLightfv(lightId, GL_POSITION, light_position);
	glEnable(lightId);
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

// MESH NODE IMPLEMENTATION
fm::MeshNode::MeshNode() : model(nullptr), material() { 
	name = "Mesh Node";
}

fm::MeshNode::MeshNode(std::string modelPath) : MeshNode() {
	model = loadObjModel(modelPath);
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
		bool usingTexture = false;

		if (texture != nullptr && texture->data != nullptr) {
			// Linear filtering
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			
			// Bind the texture using the loaded texture id
			glBindTexture(GL_TEXTURE_2D, material.texture->data->glTextureId);
			
			// Indicate that we're using the texture
			usingTexture = true;
		}

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
					//glTexCoord3f(texcoord.x, texcoord.y, texcoord.z);
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
