#include "fullmetal-helpers.h"
#include "fullmetal.h"
#include "fullmetal-gui.h"

#include "imgui\imgui.h"

#include "glut.h"


// EDITOR CAMERA CONTROLLER
fm::EditorCameraController::EditorCameraController(Camera * camera, Input * input) :
	camera(camera), _input(input), _mLastX(0), _mLastY(0), showDebugGui(true), _movementOffset() { }

void fm::EditorCameraController::useKeyControl(float dt)
{
	// movement vector
	Vector3 movement = Vector3(0.0f, 0.0f, 0.0f);

	// forward & backwards control
	if (_input->isKeyDown('w')) {
		movement += camera->forward();
	}
	else if (_input->isKeyDown('s')) {
		movement += camera->back();
	}

	// left & right control
	if (_input->isKeyDown('a')) {
		movement += camera->left();
	}
	else if (_input->isKeyDown('d')) {
		movement += camera->right();
	}

	// down & up
	if (_input->isKeyDown('z')) {
		movement += camera->down();
	}
	else if (_input->isKeyDown('x')) {
		movement += camera->up();
	}

	_movementOffset = movement;
}

void fm::EditorCameraController::useMouseControl(float dt)
{
	const float speed = 100.0f;

	// if we have space held down, we are using the mouse to control the camera
	if (_input->isKeyDown(' ')) {
		// get mouse position
		auto mouse_x = _input->getMouseX();
		auto mouse_y = _input->getMouseY();
		// get screen centre position
		auto centre_x = camera->getCentreX();
		auto centre_y = camera->getCentreY();

		// if we were not using the mouse last frame, reset mX & mY
		if (!_mouseUsed) {
			// assign last variables for mouse position
			_mLastX = mouse_x;
			_mLastY = mouse_y;

			// assign m x/y to c x/y so there is no initial mouse jump
			mouse_x = centre_x;
			mouse_y = centre_y;
		}

		// we are using the mouse
		_mouseUsed = true;

		// The direction that our camera moves
		Vector3 dir = Vector3(mouse_x, mouse_y, 0) - Vector3(centre_x, centre_y, 0);
		dir.normalise();
		dir = dir * speed * dt;

		// resets the mouse to the centre of the screen for accurate movement
		glutWarpPointer(centre_x, centre_y);

		// hide the cursor while we're using the mouse
		glutSetCursor(GLUT_CURSOR_NONE);

		// now yaw, pitch the camera by the calculated values
		camera->pitch(dir.y);
		camera->yaw(-dir.x);
	}
	else {
		// if we were using the mouse last frame..
		// reset mouse position to last known position
		if (_mouseUsed) {
			_input->setMousePos(_mLastX, _mLastY);
			glutWarpPointer(_mLastX, _mLastY);
		}

		// no longer using the mouse
		_mouseUsed = false;
		// set the cursor back on to default
		glutSetCursor(GLUT_CURSOR_INHERIT);
	}
}

void fm::EditorCameraController::debugGui(float dt)
{
#if FM_EDITOR
	ImGui::PushID("EdCam");

	if (ImGui::Begin("Camera")) {

		ImGui::InputFloat("deltatime", &dt);
		
		Vector3 pos = camera->getPosition();
		Vector3 rot = camera->getRotation();

		// show camera position..
		ImGui::Text("Position");
		ImGui::Indent();

		if (ImGui::DragFloat("x", &pos.x) || ImGui::DragFloat("y", &pos.y) || 
			ImGui::DragFloat("z", &pos.z)) {
			camera->setPosition(pos);
		}
		
		ImGui::Unindent();

		// show camera orientation..
		ImGui::Text("Rotation");
		ImGui::Indent();

		if (ImGui::DragFloat("pitch (x)", &rot.x) || ImGui::DragFloat("yaw (y)", &rot.y) || 
			ImGui::DragFloat("roll (z)", &rot.z))  {
			camera->setOrientation(rot);
		}
		
		ImGui::Unindent();

		// fov & planes
		ImGui::Text("Other");
		ImGui::Indent();

		int screenW = camera->getScreenWidth();
		ImGui::DragInt("width", &screenW);

		int screenH = camera->getScreenHeight();
		ImGui::DragInt("height", &screenH);

		float fov = camera->getFov();
		ImGui::DragFloat("fov", &fov);

		float nearPlane = camera->getNearPlane();
		ImGui::DragFloat("near plane", &nearPlane);

		float farPlane = camera->getFarPlane();
		ImGui::DragFloat("far plane", &farPlane);

		ImGui::Unindent();

		// directions..
		ImGui::Text("Directions");
		ImGui::Indent();
		fm::gui::introspectVector3(camera->up(), "up");
		fm::gui::introspectVector3(camera->forward(), "forward");
		fm::gui::introspectVector3(camera->right(), "right");
		ImGui::Unindent();

		if (ImGui::Button("Reset"))
			camera->reset();

		ImGui::End();
	}

	ImGui::PopID();
#endif
}

bool fm::EditorCameraController::isUsingMouse()
{
	return _mouseUsed;
}

void fm::EditorCameraController::update(Camera * camera, float dt)
{
	if (showDebugGui) {
		debugGui(dt);
	}

	// use WASD to move the camera 
	useKeyControl(dt);

	// use mouse to rotate/look with the camera
	useMouseControl(dt);

	if (!_movementOffset.isZero()) {
		// normalise, apply speed & time, offset camera position
		_movementOffset.normalise();
		_movementOffset = _movementOffset * 5 * dt;
		camera->move(_movementOffset);
		
		// reset offset
		_movementOffset = Vector3();
	}
}

void fm::EditorCameraController::start(Camera * camera) {}

// MOVE TO CAMERA CONTROLLER
fm::MoveToCameraController::MoveToCameraController(Vector3 destination) 
	: _destination(destination) { }

void fm::MoveToCameraController::update(Camera * camera, float dt)
{
	_lerpTime += dt;

	if (_lerpTime >= 1.0f) {
		camera->setPosition(_destination);
		camera->popController();
		return;
	}
	else
		camera->setPosition(lerp_vector(_start, _destination, smooth_lerp(_lerpTime, 1.0f)));
}

void fm::MoveToCameraController::start(Camera * camera)
{
	// Set start to camera position
	_start = camera->getPosition();
	_lerpTime = 0.0f;
}

// LERPING
float fm::linear_lerp(const float start, const float end)
{
	return start / end;
}

float fm::smooth_lerp(const float start, const float end)
{
	float t = start / end;
	return t * t * (3.f - 2.f * t);
}

fm::Vector3 fm::lerp_vector(Vector3 & start, Vector3 & end, const float lerp)
{
	// get end to start..
	Vector3 dir = end - start;
	Vector3 val = start + (dir * lerp);
	return val;
}

// SKYBOX
void fm::renderSkybox(Camera * camera)
{
	// the texture cube we use as a skybox
	static CubeNode* cube = new CubeNode("Assets//gfx//skybox.png");
	
	// set cube position to our camera
	cube->transform.position = camera->getPosition();
	
	// now we render the skybox..
	glDisable(GL_DEPTH_TEST);
	cube->render();
	glEnable(GL_DEPTH_TEST);
}
