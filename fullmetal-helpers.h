#pragma once

/*
 * General helpers and extensions for fullmetal. Including: 
 * - Editor Camera Controller
 * - ....
 * - ....
 */

namespace fm
{
	class Camera;
	class Input;
	class Vector3;

	/*
	 * A camera controller for the editor mode of fullmetal.
	 */
	class EditorCameraController {
	private:
		bool _mouseUsed;
		Camera* camera;
		Input* input;

	public:
		EditorCameraController(Camera* camera, Input* input);

		/*
		 * If the camera is currently using mouse control.
		 */
		bool isUsingMouse();

		/*
		 * Controls the given camera. Listens to user input in order to control it.
		 */
		void update(float dt);

		/*
		 * Moves the camera to look at a specific point in 3d space.
		 */
		void lookTowards(Vector3* transform);
	};

	float smoothlerp(float start, float end);
}
