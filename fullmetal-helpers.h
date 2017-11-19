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
	class EditorCameraController : public CameraController  {
	private:
		Camera* camera;
		Input* _input;

		bool _mouseUsed;
		int _mLastX, _mLastY;

		Vector3 _movementOffset;

		void useKeyControl(float dt);
		void useMouseControl(float dt);

		void debugGui(float dt);

	public:
		bool showDebugGui;

		EditorCameraController(Camera* camera, Input* input);

		/*
		 * If the camera is currently using mouse control.
		 */
		bool isUsingMouse();

		void update(Camera* camera, float dt) override;
		void start(Camera* camera) override;
	};

	/*
	 * Camera controller that moves from A to B using smooth_lerp.
	 */
	class MoveToCameraController : public CameraController {
	private:
		Vector3 _destination;
		Vector3 _start;

		float _lerpTime;

	public:
		MoveToCameraController(Vector3 destination);
		void update(Camera* camera, float dt) override;
		void start(Camera* camera) override;
	};

	// Linear lerp
	float linear_lerp(const float start, const float end);

	// Smooth lerp
	float smooth_lerp(const float start, const float end);

	// Returns a vector equal to 'lerp' between start and destination.
	fm::Vector3 lerp_vector(Vector3& start, Vector3& destination, const float lerp);

	// Renders the skybox
	void renderSkybox(Camera* camera);
}
