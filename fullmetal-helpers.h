#pragma once

/*
 * General helpers and extensions for fullmetal. Including: 
 *  Editor Camera Controller
 *  Lerp functions
 *  
 *  
 */

#include "fullmetal.h"

namespace fm
{
	class Camera;
	class Input;
	class Vector3;
	class ObjModel;

	/*
	 * A camera controller for the editor mode of fullmetal.
	 * Expects an input object and a camera object, keeps references to
	 * but deletes neither object.
	 */
	class EditorCameraController {
	private:
		Camera* camera;

		bool _mouseUsed;
		Input* input;
		int _mLastX, _mLastY;

		bool _lerping;
		Vector3 _lerpTarget;
		float _lerpDt;

		void useKeyControl(float dt);
		void useMouseControl(float dt);

		void lerpCamera(float dt);

		void debugGui(float dt);

	public:
		bool showDebugGui;

		EditorCameraController(Camera* camera, Input* input);

		/*
		 * Begins movement towards a given position.
		 */
		void lerp_to(const Vector3& v);

		/*
		 * If the camera is currently using mouse control.
		 */
		bool isUsingMouse();

		/*
		 * Controls the given camera. Listens to user input in order to control it.
		 */
		void update(float dt);
	};

	// Linear lerp
	float linear_lerp(const float start, const float end);

	// Smooth lerp
	float smooth_lerp(const float start, const float end);

	fm::Vector3 lerp_vector(Vector3& start, Vector3& destination, const float lerp);
}
