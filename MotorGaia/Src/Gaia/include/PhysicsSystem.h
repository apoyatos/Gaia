#pragma once
#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include <map>
#include <vector>

#include "Vector3.h"
#include "Singleton.h"
#include "PhysicsUtils.h"

class DebugDrawer;
class Transform;
class GaiaMotionState;
class RigidBody;
class MeshStrider;

class btRigidBody;
class btTransform;
class btCollisionObject;
class btDiscreteDynamicsWorld;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btCollisionShape;
class RaycastHit;

class GAIA_API PhysicsSystem : public Singleton<PhysicsSystem>
{
	friend class GaiaCore;
	friend class SceneManager;
	friend class RigidBody;
	friend class Strider;
	friend class GaiaMotionState;
public:
	PhysicsSystem();
	~PhysicsSystem();

private:
	void init();
	void render();
	void update(float deltaTime);
	void postUpdate();
	void close();

	void clearWorld();

public:
	// World config methods
	void setWorldGravity(Vector3 gravity);
	Vector3 getWorldGravity() const;

	std::vector<RaycastHit> raycastAll(const Vector3& from, const Vector3& to, uint16_t mask = ALL);
	std::vector<RaycastHit> raycastAll(const Vector3& from, const Vector3& dir, float maxDistance, uint16_t mask = ALL);
	bool raycast(const Vector3& from, const Vector3& to, RaycastHit& hit, uint16_t mask = ALL);
	bool raycast(const Vector3& from, const Vector3& dir, float maxDistance, RaycastHit& hit, uint16_t mask = ALL);

	void drawLine(const Vector3& ini, const Vector3& end, const Vector3& color);

private:
	void setDebugDrawer(DebugDrawer* debugDrawer);

	// Rigid Body methods
	btRigidBody* createRigidBody(float m, RB_Shape shape, GaiaMotionState* mState, Vector3 dim, uint16_t myGroup, uint16_t collidesWith);
	void deleteRigidBody(btRigidBody* body);
	// Turns a Gaia Transform into a Bullet Physics Transform
	btTransform parseToBulletTransform(Transform* transform);
	btRigidBody* bodyFromStrider(MeshStrider* strider, GaiaMotionState* mState, const Vector3& dim, uint16_t myGroup, uint16_t collidesWith);
	void checkCollisions();

	void CollisionEnterCallbacks(const std::pair<RigidBody*, RigidBody*>& col);
	void CollisionExitCallbacks(const std::pair<RigidBody*, RigidBody*>& col);
	void CollisionStayCallbacks(const std::pair<RigidBody*, RigidBody*>& col);

	void deleteBody(btCollisionObject* obj);

	btDiscreteDynamicsWorld* dynamicsWorld;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;

	//keep track of the shapes, we release memory at exit.
	//make sure to re-use collision shapes among rigid bodies whenever possible!
	std::vector<btCollisionShape*> collisionShapes;

	std::map<std::pair<RigidBody*, RigidBody*>, bool> contacts;

	float timeAccumulator;
};

#endif