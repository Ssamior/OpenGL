#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

/**************************************************************************\
*                                                                          *
* Default OpenGL camera (with rotation (x,y,z)=(0,0,0)) has an up vector   *
* set to (0,1,0) and a direction vector set to (0,0,-1). The position is   *
* (0,0,0). Without rotation, the screen viewport is in the plane XY.       *
* Ex: http://www.songho.ca/opengl/files/gl_camera01.png                    *
*                                                                          *
\**************************************************************************/
Camera::Camera()
	: m_defaultForward(glm::vec3(0,0,-1))		// (0,0,-1) : "OpenGL compatible"
	, m_defaultUp(glm::vec3(0,1,0))				// (0,1, 0) : "OpenGL compatible"
	, m_Yaw  (0.0f)
	, m_Pitch(0.0f)
	, m_Roll (0.0f)
	, m_MovementSpeed(50.0f)
	, m_MouseSensitivity(0.2f)
	, m_ViewAngle(45.0f)
	, m_ZClippingNear(10.0f)
	, m_ZClippingFar(20000.0f)
	, m_WindowWidth(800)
	, m_WindowHeight(600)
{
	SetPosition(0,0,0);
	SetRotation(0,0);
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
glm::mat4 Camera::GetViewMatrix() const
{
	return m_ViewMatrix;
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
glm::mat4 Camera::GetProjMatrix() const
{
	return m_ProjMatrix;
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
void Camera::Move(CameraMovement direction, float deltaTime)
{
	float velocity = m_MovementSpeed * deltaTime;

	switch (direction)
	{
		case FORWARD:	m_Position += m_forwardAxis * velocity; break;
		case BACKWARD:	m_Position -= m_forwardAxis * velocity; break;
		case RIGHT:		m_Position += m_rightAxis   * velocity; break;
		case LEFT:		m_Position -= m_rightAxis   * velocity; break;
		case UP:		m_Position += m_upAxis      * velocity; break;
		case DOWN:		m_Position -= m_upAxis      * velocity; break;
		default:		return;
	}

	UpdateView();
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
void Camera::Rotate(float dxAngle, float dyAngle)
{
	m_Yaw   += dxAngle * m_MouseSensitivity;
	m_Pitch += dyAngle * m_MouseSensitivity;
	SetRotation(m_Yaw, m_Pitch);
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
void Camera::SetPosition(float x, float y, float z)
{
	m_Position = glm::vec3(x,y,z);
	UpdateView();
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
void Camera::SetRotation(float x, float y)
{
	m_Yaw   = x;
	m_Pitch = y;
	while (m_Yaw   >  360.0f) m_Yaw   -= 360.0f;
	while (m_Yaw   < -360.0f) m_Yaw   += 360.0f;
	while (m_Pitch >  360.0f) m_Pitch -= 360.0f;
	while (m_Pitch < -360.0f) m_Pitch += 360.0f;

	m_forwardAxis	= quaternionRotation(m_defaultForward, m_Yaw, m_Pitch, m_Roll);
	m_upAxis		= quaternionRotation(m_defaultUp,      m_Yaw, m_Pitch, m_Roll);
	m_rightAxis		= glm::cross(m_forwardAxis,m_upAxis);

	// Update Front, Right and Up Vectors using the updated Eular angles
	UpdateView();
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
void Camera::SetSize(int windowWidth, int windowHeight)
{
	m_WindowWidth  = (float)windowWidth;
	m_WindowHeight = (float)windowHeight;
	UpdateProj();
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
void Camera::SetViewAngle(float fieldOfView)
{
	m_ViewAngle = fieldOfView;
	UpdateProj();
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
void Camera::SetClipping(float Z_near, float Z_far)
{
	// DO NOT PUT A SMALL VALUE FOR Z_near, OR YOUR Z-BUFFER WILL HAVE A VERY BAD RESOLUTION !
	// Z_far is less critical, but don't put too much too.
	// Example : use 10 & 20000, and imagine it's in cm : Clipping near=10 cm, and clipping far=200 m.

	m_ZClippingNear = Z_near;
	m_ZClippingFar  = Z_far;
	UpdateProj();
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
void Camera::UpdateView()
{
	m_ViewMatrix = glm::lookAt(m_Position, m_Position+m_forwardAxis, m_upAxis);
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
void Camera::UpdateProj()
{
	float windowAspectRatio = m_WindowWidth / m_WindowHeight;
	m_ProjMatrix = glm::perspective(glm::radians(m_ViewAngle), windowAspectRatio, m_ZClippingNear, m_ZClippingFar);
}

/**************************************************************************\
*                                                                          *
*   Rotate a point or a unit vector around standard axis                   *
*                                                                          *
\**************************************************************************/
glm::vec3 Camera::quaternionRotation(const glm::vec3& p, float angleX, float angleY, float angleZ)
{
	float tx = angleX*M_PI_360f;	// angle/2 converted to radians
	float ty = angleY*M_PI_360f;
	float tz = angleZ*M_PI_360f;

	quaternion q1(sinf(tx), 0,        0,        cosf(tx));
	quaternion q2(0,        sinf(ty), 0,        cosf(ty));
	quaternion q3(0,        0,        sinf(tz), cosf(tz));

	quaternion P(p.x, p.y, p.z, 0.0f);
	quaternion tmp1 = q1 * P    * q1.inv();
	quaternion tmp2 = q2 * tmp1 * q2.inv();
	quaternion tmp3 = q3 * tmp2 * q3.inv();

	return glm::vec3(tmp3.x,tmp3.y,tmp3.z);
/*
	glm::quat q1 = glm::angleAxis(tx, glm::vec3(sinf(tx), 0.0f,     0.0f    ));
	glm::quat q2 = glm::angleAxis(ty, glm::vec3(0.0f,     sinf(ty), 0.0f    ));
	glm::quat q3 = glm::angleAxis(tz, glm::vec3(0.0f,     0.0f,     sinf(tz)));

	glm::quat P = glm::angleAxis(0.f,p);
	glm::quat tmp1 = q1 * P    * glm::inverse(q1);
	glm::quat tmp2 = q2 * tmp1 * glm::inverse(q2);
	glm::quat tmp3 = q3 * tmp2 * glm::inverse(q3);
//	glm::quat tmp1 = glm::inverse(q1) * P    * q1;
//	glm::quat tmp2 = glm::inverse(q2) * tmp1 * q2;
//	glm::quat tmp3 = glm::inverse(q3) * tmp2 * q3;

	return glm::axis(tmp3);
*/
}

quaternion::quaternion()
{
	s = 0.0f;
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

quaternion::quaternion(float X, float Y, float Z, float S)
{
	s = S;
	x = X;
	y = Y;
	z = Z;
}

quaternion quaternion::operator*(const quaternion& q)
{
	float Qs = s * q.s - ( x * q.x + y * q.y + z * q.z );
	float Qx = (s * q.x + q.s * x ) + ( y * q.z - z * q.y );
	float Qy = (s * q.y + q.s * y ) + ( z * q.x - x * q.z );
	float Qz = (s * q.z + q.s * z ) + ( x * q.y - y * q.x );

	return quaternion(Qx,Qy,Qz,Qs);
}

quaternion quaternion::inv() const
{
	return quaternion(-x, -y, -z, s);
}
