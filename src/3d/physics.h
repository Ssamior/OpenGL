#pragma once
#include "../types.h"
#include "bullet/btBulletDynamicsCommon.h"
#include "bullet/LinearMath/btVector3.h"
#include "bullet/LinearMath/btMatrix3x3.h"
#include "bullet/LinearMath/btTransform.h"
#include "bullet/LinearMath/btQuickprof.h"
#include "bullet/LinearMath/btAlignedObjectArray.h"
#include "bullet/LinearMath/btHashMap.h"
class Model;

class Physics
{
	private:
		btRigidBody* AddDynamicObjetToPhysic(btCollisionShape* pShape,glm::mat4 transform, glm::vec3 inertia, float mass, float friction);
		btRigidBody* AddStaticObjetToPhysic (btCollisionShape* pShape,glm::mat4 transform);
		void		 InitData();
		btTransform	 glmMat4TobtTransform(const glm::mat4& transform);
		glm::mat4	 btTransformToGlmMat4(const btTransform& transform);

		btDefaultCollisionConfiguration*		m_pCollisionConfiguration;
		btCollisionDispatcher*					m_pCollisionDispatcher;
		btBroadphaseInterface*					m_pBroadphaseItf;
		btSequentialImpulseConstraintSolver*	m_pSolver;
		btDiscreteDynamicsWorld*				m_pDynamicsWorld;
//		vector<btCollisionShape*>				m_btShapes;
		map<btRigidBody*,pair<glm::mat4,btCollisionShape*>> m_btObjects;	// Objets ajout�s


	public:
		 Physics();
		~Physics();

		void		Initialize();
		void		Terminate();

		// Ajout / retrait d'objets
		btRigidBody* AddDynamicSphereToPhysic     (float radius,        glm::mat4 transform, glm::vec3 inertia, float mass, float friction);
		btRigidBody* AddDynamicConvexHullToPhysic (const Model* pModel, glm::mat4 transform, glm::vec3 inertia, float mass, float friction);
		btRigidBody* AddStaticConcaveObjetToPhysic(const Model* pModel, glm::mat4 transform);
		btRigidBody* AddStaticPlaneObjetToPhysic  ();
		void		 RemoveObject(btRigidBody* pBody);

		// Get/Set des transformations des objets
		glm::mat4	GetObjectUpdatedMatrix(btRigidBody* pBody);
		void		SetObjectMatrix(btRigidBody* pBody, const glm::mat4& mat);

		// M�j de la vitesse des objets
		void		SetVelocity(btRigidBody* pBody, glm::vec3 direction);

		// M�j du moteur physique
		void		UpdatePhysics(float deltaTimeInSecond);
};
