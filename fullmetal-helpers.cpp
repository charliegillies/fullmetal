#include "fullmetal-helpers.h"
#include "fullmetal.h"
#include "fullmetal-gui.h"

#include "imgui\imgui.h"

#include "glut.h"


fm::EditorCameraController::EditorCameraController(Camera * camera, Input * input) :
	camera(camera), input(input), _mLastX(0), _mLastY(0),
	showDebugGui(true), _lerping(false), _lerpDt(0.0f) { }

void fm::EditorCameraController::lerp_to(const Vector3 & end)
{
	_lerping = true;
	_lerpDt = 0.0f;
	_lerpTarget = end;
}

void fm::EditorCameraController::useKeyControl(float dt)
{
	// movement vector
	Vector3 movement = Vector3(0.0f, 0.0f, 0.0f);

	// forward & backwards control
	if (input->isKeyDown('w')) {
		movement += camera->forward();
	}
	else if (input->isKeyDown('s')) {
		movement += camera->back();
	}

	// left & right control
	if (input->isKeyDown('a')) {
		movement += camera->left();
		input->SetKeyUp('a');
	}
	else if (input->isKeyDown('d')) {
		movement += camera->right();
		input->SetKeyUp('d');
	}

	// down & up
	if (input->isKeyDown('z')) {
		movement += camera->down();
	}
	else if (input->isKeyDown('x')) {
		movement += camera->up();
	}

	// if we actually applied anything..
	if (!movement.isZero()) {
		// don't need to normalise, left/right/down/up are all unit vectors
		// so just multiply by scroll speed and deltatime
		Vector3 camPos = camera->getPosition();
		lerp_to(camPos + movement);
	}
}

void fm::EditorCameraController::useMouseControl(float dt)
{
	const float speed = 100.0f;

	// if we have space held down, we are using the mouse to control the camera
	if (input->isKeyDown(' ')) {
		// get mouse position
		auto mouse_x = input->getMouseX();
		auto mouse_y = input->getMouseY();
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
		Vector3 dir = (Vector3(mouse_x, mouse_y, 0) - Vector3(centre_x, centre_y, 0)).normalised();
		dir = dir * speed;
		dir = dir * dt;

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
			input->setMousePos(_mLastX, _mLastY);
			glutWarpPointer(_mLastX, _mLastY);
		}

		// no longer using the mouse
		_mouseUsed = false;
		// set the cursor back on to default
		glutSetCursor(GLUT_CURSOR_INHERIT);
	}
}

void fm::EditorCameraController::lerpCamera(float dt)
{
	const float maxLerp = 0.8f;
	_lerpDt += dt;

	if (_lerpDt >= maxLerp) {
		_lerping = false;
		_lerpDt = 0.0f;
	}
	else {
		Vector3 camPos = camera->getPosition();
		Vector3 pos = lerp_vector(camPos, _lerpTarget, linear_lerp(_lerpDt, maxLerp));
		camera->setPosition(pos);
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

void fm::EditorCameraController::update(float dt)
{
	if (showDebugGui) {
		debugGui(dt);
	}

	if (_lerping) {
		lerpCamera(dt);
	}

	// use WASD to move the camera 
	useKeyControl(dt);

	// use mouse to rotate/look with the camera
	useMouseControl(dt);
}

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
