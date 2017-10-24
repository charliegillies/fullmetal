#include "fullmetal-helpers.h"
#include "fullmetal.h"

#include "glut.h"

fm::EditorCameraController::EditorCameraController(Camera * camera, Input * input) 
	: camera(camera), input(input) { }

bool fm::EditorCameraController::isUsingMouse()
{
	return _mouseUsed;
}

void fm::EditorCameraController::update(float dt)
{
	const float speed = 2.0f;

	// forward & backwards control
	if (input->isKeyDown('w')) {
		auto forward = camera->forward();
		forward = (forward * speed) * dt;
		camera->move(forward);
	}
	else if (input->isKeyDown('s')) {
		auto backwards = camera->back();
		backwards = (backwards * speed) * dt;
		camera->move(backwards);
	}

	// left & right control
	if (input->isKeyDown('a')) {
		auto left = camera->left();
		left = (left * speed) * dt;
		camera->move(left);
	}
	else if (input->isKeyDown('d')) {
		auto right = camera->right();
		right = (right * speed) * dt;
		camera->move(right);
	}

	// if we have space held down, we are using the mouse to control the camera
	if (input->isKeyDown(' ')) {
		// get mouse position
		auto mouse_x = input->getMouseX();
		auto mouse_y = input->getMouseY();
		// get screen centre position
		auto centre_x = camera->getCentreX();
		auto centre_y = camera->getCentreY();

		// if we were not using the mouse last frame, reset mX & mY
		// so we don't get a huge jump in the wrong direction
		if (!_mouseUsed) {
			mouse_x = centre_x;
			mouse_y = centre_y;
		}

		// we are using the mouse
		_mouseUsed = true;

		// The direction that our camera moves
		Vector3 dir = Vector3(mouse_x, mouse_y, 0) - Vector3(centre_x, centre_y, 0);
		dir = dir * speed * 5;
		dir = dir * dt;

		// resets the mouse to the centre of the screen for accurate movement
		glutWarpPointer(centre_x, centre_y);

		// hide the cursor while we're using the mouse
		glutSetCursor(GLUT_CURSOR_NONE);

		// now yaw, pitch the camera by the calculated values
		camera->yaw(dir.x);
		camera->pitch(dir.y);
	}
	else {
		// no longer using the mouse
		_mouseUsed = false;
		// set the cursor back on to default
		glutSetCursor(GLUT_CURSOR_INHERIT);
	}
}

void fm::EditorCameraController::lookTowards(Vector3 * transform)
{
}

float fm::smoothlerp(float start, float end)
{
	float t = start / end;
	return t * t * (3.f - 2.f * t);
}
