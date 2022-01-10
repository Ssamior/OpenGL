#pragma once
#include "../types.h"

enum CameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class quaternion
{
	public:

		quaternion();
		quaternion(float X, float Y, float Z, float S);

		quaternion	operator*(const quaternion&);
		quaternion	inv() const;
//		void		setup_rotation(const point& vecteur, float angle);

		float	s;
		float	x;
		float	y;
		float	z;
};

class Camera
{
	private:
		glm::vec3	m_defaultForward;	// (0,0,-1) : "OpenGL compatible"
		glm::vec3	m_defaultUp;		// (0,1, 0) : "OpenGL compatible"
		glm::mat4	m_ViewMatrix;
		glm::mat4	m_ProjMatrix;

		void		UpdateView();		// Move/rotate the camera
		void		UpdateProj();		// Change the projection (how 3D objects in space are projected on a flat screen)
		glm::vec3	quaternionRotation(const glm::vec3& p, float angleX, float angleY, float angleZ);


	public:
		// Camera attributes
		glm::vec3	m_Position;			// 3D Position
		glm::vec3	m_forwardAxis;		// rotated forward axis
		glm::vec3	m_upAxis;			// rotated up axis
		glm::vec3	m_rightAxis;		// rotated right axis


		// Euler angles
		float		m_Yaw;
		float		m_Pitch;
		float		m_Roll;

		// Camera parameters
		float		m_MovementSpeed;
		float		m_MouseSensitivity;
		float		m_ViewAngle;
		float		m_ZClippingNear;
		float		m_ZClippingFar;
		float		m_WindowWidth;
		float		m_WindowHeight;


		// Constructor
		Camera();

		glm::mat4	GetViewMatrix() const;								// Get view matrix.
		glm::mat4	GetProjMatrix() const;								// Get projection matrix.

		void		Move(CameraMovement direction, float deltaTime);	// Move   (relative to the current position).
		void		Rotate(float xAngle, float yAngle);					// Rotate (relative to the current orientation).
		void		SetPosition(float x, float y, float z);				// Set fixed values.
		void		SetRotation(float x, float y);						// Set fixed values.

		void		SetSize(int windowWidth, int windowHeight);			// Update the camera when the window changes its size.
		void		SetViewAngle(float fieldOfView);					// Update field of view angle.
		void		SetClipping(float Z_near, float Z_far);				// Update clipping planes.
};
