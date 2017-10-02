#include "fullmetal.h"
#include "glut.h"
#include <math.h>
#include <gl/GL.h>
#include <gl/GLU.h>

// GL HELPERS
void fm::fmApplyColor(Color& color)
{
	glColor4f(color.r, color.g, color.b, color.a);
}

void fm::fmApplyTransform(Transform& transform)
{
	glRotatef(transform.angle, transform.rotation.x, transform.rotation.y, transform.rotation.z);
	glTranslatef(transform.position.x, transform.position.y, transform.position.z);
	glScalef(transform.scale.x, transform.scale.y, transform.scale.z);
}

void fm::fmNormalVertex(const Vector3 & normal, float x, float y, float z)
{
	glNormal3f(normal.x, normal.y, normal.z);
	glVertex3f(x, y, z);
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

// TRANSFORM IMPLEMENTATION
fm::Transform::Transform() : position(1, 1, 1), scale(1, 1, 1), rotation(0, 0, 0), angle(0.0f) { }

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


// COLOR IMPLEMENTATION
fm::Color::Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {
	clamp(r);
	clamp(g);
	clamp(b);
	clamp(a);
}

fm::Color::Color(float r, float g, float b) : Color(r, g, b, 1) { }

fm::Color::Color() : Color(1, 1, 1, 1) { }

void fm::Color::clamp(float & v)
{
	if (v < 0.0f)
		v = 0.0f;
	else if (v > 1.0f)
		v = 1.0f;
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
	for (auto node : _nodes)
		node->render();
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

// SCENE NODE IMPLEMENTATION
fm::SceneNode::SceneNode()
{
	static unsigned int globalUID = 0;
	++globalUID;
	_uid = globalUID;

	name = "Scene Node";
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
	for (auto child : childNodes)
		child->render();
}

void fm::SceneNode::addChild(SceneNode * child)
{
	childNodes.push_back(child);
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
fm::ShapeNode::ShapeNode(Color color) : color(color) {
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
	fmApplyTransform(transform);
	fmApplyColor(color);

	glBegin(GL_QUADS);

		//Front
		const Vector3 frontNormal(0.0f, 0.0f, 1.0f);
		fmNormalVertex(frontNormal, -1.0f, -1.0f, 1.0f);
		fmNormalVertex(frontNormal, 1.0f, -1.0f, 1.0f);
		fmNormalVertex(frontNormal, 1.0f, 1.0f, 1.0f);
		fmNormalVertex(frontNormal, -1.0f, 1.0f, 1.0f);

		// Right
		//fmColor(Color(0.5f, 0.0f, 0.5f, 1.0f));
		const Vector3 rightNormal(1.0f, 0.0f, 0.0f);
		fmNormalVertex(rightNormal, 1.0f, -1.0f, -1.0f);
		fmNormalVertex(rightNormal, 1.0f, 1.0f, -1.0f);
		fmNormalVertex(rightNormal, 1.0f, 1.0f, 1.0f);
		fmNormalVertex(rightNormal, 1.0f, -1.0f, 1.0f);

		// Bottom
		//fmColor(Color(0.0f, 0.5f, 0.5f, 1.0f));
		const Vector3 bottomNormal(0.0f, -1.0f, 0.0f);
		fmNormalVertex(bottomNormal, 1.0f, -1.0f, 1.0f);
		fmNormalVertex(bottomNormal, 1.0f, -1.0f, -1.0f);
		fmNormalVertex(bottomNormal, -1.0f, -1.0f, -1.0f);
		fmNormalVertex(bottomNormal, -1.0f, -1.0f, 1.0f);

		// Left
		//fmColor(Color(0.0f, 0, 1, 1.0f));
		const Vector3 leftNormal(-1.0f, 0.0f, 0.0f);
		fmNormalVertex(leftNormal, -1.0f, -1.0f, 1.0f);
		fmNormalVertex(leftNormal, -1.0f, -1.0f, -1.0f);
		fmNormalVertex(leftNormal, -1.0f, 1.0f, -1.0f);
		fmNormalVertex(leftNormal, -1.0f, 1.0f, 1.0f);

		// Top
		//fmColor(Color(0.0f, 1, 0, 1.0f));
		const Vector3 topNormal(0, 1, 0);
		fmNormalVertex(topNormal, -1.0f, 1.0f, 1.0f);
		fmNormalVertex(topNormal, 1.0f, 1.0f, 1.0f);
		fmNormalVertex(topNormal, 1.0f, 1.0f, -1.0f);
		fmNormalVertex(topNormal, -1.0f, 1.0f, -1.0f);

		// Back
		//fmColor(Color(1.0f, 0, 0, 1.0f));
		const Vector3 backNormal(0, 0, -1);
		fmNormalVertex(backNormal, -1.0f, -1.0f, -1.0f);
		fmNormalVertex(backNormal, 1.0f, -1.0f, -1.0f);
		fmNormalVertex(backNormal, 1.0f, 1.0f, -1.0f);
		fmNormalVertex(backNormal, -1.0f, 1.0f, -1.0f);

	glEnd();

	// Draw children
	SceneNode::render();

	glPopMatrix();
}

// SPHERE NODE IMPLEMENTATION
fm::SphereNode::SphereNode(Color color) : ShapeNode(color) {
	name = "Sphere Node";
}

fm::SphereNode::SphereNode() : SphereNode(Color(1, 1, 1, 1)) { }

void fm::SphereNode::render()
{
	glPushMatrix();
	fmApplyTransform(transform);
	fmApplyColor(color);

		gluSphere(gluNewQuadric(), 1, 20, 20);

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
	fmApplyTransform(transform);
	fmApplyColor(color);

	glBegin(GL_TRIANGLES);

	// now write every triangle we built with buildQuads()
	for (int i = 0; i < _tris.size(); ++i) {
		Tri& tri = _tris[i];
		glVertex3d(tri.v1.x, tri.v1.y, tri.v1.z);
		glVertex3d(tri.v2.x, tri.v2.y, tri.v2.z);
		glVertex3d(tri.v3.x, tri.v3.y, tri.v3.z);
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

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			Vector3 v1 = Vector3(x, 0, y);
			Vector3 v2 = Vector3(x + size, 0, y);
			Vector3 v3 = Vector3(x + size, 0, y + size);
			Vector3 v4 = Vector3(x, 0, y + size);

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

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
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
	
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position);
	glEnable(GL_LIGHT1);
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

	glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT2, GL_POSITION, light_position);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, cutoff);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, exponent);
	glEnable(GL_LIGHT2);
}
