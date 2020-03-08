#include "RigidBody.h"
#include "btBulletDynamicsCommon.h"
#include "GameObject.h"
#include "GaiaMotionState.h"
#include "ComponentData.h"

Vector3 RigidBody::btScaleConversion = { 50,50,50 };

RigidBody::RigidBody(GameObject* gameObject) : GaiaComponent(gameObject)
{
}


RigidBody::~RigidBody()
{
}

void RigidBody::setRigidBody(float mass, RB_Shape shape, const Vector3& offset, const Vector3& dim, bool isT)
{
	motionState = new GaiaMotionState(gameObject->transform, offset);
	body = PhysicsSystem::GetInstance()->createRigidBody(mass, shape, motionState, gameObject->transform->getScale() * dim * btScaleConversion);
	body->setCollisionFlags(body->getCollisionFlags() | (body->CF_NO_CONTACT_RESPONSE * isT));
	body->setUserPointer(this);
	trigger = isT;
}

void RigidBody::handleData(ComponentData* data)
{
	float mass = 1.0;
	RB_Shape shape;
	Vector3 off = Vector3(), dim = Vector3(1,1,1);
	bool isTrigger = false;

	for (auto prop : data->getProperties()) {
		std::stringstream ss(prop.second);

		if (prop.first == "shape") {
			if (prop.second == "Box")
				shape = BOX_RB_SHAPE;
			else if (prop.second == "Sphere")
				shape = SPHERE_RB_SHAPE;
			else
				printf("RIGIDBODY: %s not valid rigidbody shape type\n", prop.second.c_str());
		}
		else if (prop.first == "mass") {
			ss >> mass;
		}
		else if (prop.first == "offset") {
			ss >> off.x >> off.y >> off.z;
		}
		else if (prop.first == "size") {
			ss >> dim.x >> dim.y >> dim.z;
		}
		else if (prop.first == "trigger") {
			ss >> isTrigger;
		}
	}
	setRigidBody(mass, shape, off, dim, isTrigger);
}

bool RigidBody::isTrigger() const
{
	return trigger;
}

void RigidBody::addForce(const Vector3& force, Vector3 rel_pos)
{
	if (rel_pos == Vector3(0.0f, 0.0f, 0.0f))
		body->applyCentralForce(parseToBulletVector(force));
	else
		body->applyForce(parseToBulletVector(force), parseToBulletVector(rel_pos));
}

void RigidBody::addImpulse(const Vector3& impulse, ImpulseMode mode, Vector3 rel_pos)
{
	switch (mode)
	{
	case IMPULSE:
		if (rel_pos == Vector3(0.0f, 0.0f, 0.0f))
			body->applyCentralImpulse(parseToBulletVector(impulse));
		else
			body->applyImpulse(parseToBulletVector(impulse), parseToBulletVector(rel_pos));
		break;
	case PUSH:
		body->applyPushImpulse(parseToBulletVector(impulse), parseToBulletVector(rel_pos));
		break;
	case TORQUE:
		body->applyTorqueImpulse(parseToBulletVector(impulse));
		break;
	case TORQUE_TURN:
		body->applyTorqueTurnImpulse(parseToBulletVector(impulse));
		break;
	}
}

void RigidBody::addTorque(const Vector3& torque)
{
	body->applyTorque(parseToBulletVector(torque));
}

void RigidBody::setGravity(const Vector3& grav)
{
	body->setGravity(parseToBulletVector(grav));
}

void RigidBody::setDamping(float damping)
{
	body->setDamping(btScalar(damping), body->getAngularDamping());
}

void RigidBody::setAngularDamping(float damping)
{
	body->setDamping(body->getLinearDamping(), btScalar(damping));
}

void RigidBody::setAngularVelocity(const Vector3& ang_vel)
{
	body->setAngularVelocity(parseToBulletVector(ang_vel));
}

void RigidBody::setLinearVelocity(const Vector3& vel)
{
	body->setLinearVelocity(parseToBulletVector(vel));
}

void RigidBody::setFriction(float friction)
{
	body->setFriction(friction);
}

void RigidBody::setRestitution(float restitution)
{
	body->setRestitution(restitution);
}

const Vector3& RigidBody::getGravity() const
{
	btVector3 btg = body->getGravity();
	return Vector3(double(btg.x()), double(btg.y()), double(btg.z()));
}

float RigidBody::getLinearDamping()
{
	return body->getLinearDamping();
}

bool RigidBody::isActive()
{
	return body->isActive();
}

const Vector3& RigidBody::getAngularVelocity() const
{
	btVector3 btav = body->getAngularVelocity();
	return Vector3(double(btav.x()), double(btav.y()), double(btav.z()));
}

float RigidBody::getFriction()
{
	return body->getFriction();
}

const Vector3& RigidBody::getLinearVelocity() const
{
	btVector3 btlv = body->getLinearVelocity();
	return Vector3(double(btlv.x()), double(btlv.y()), double(btlv.z()));
}

float RigidBody::getRestitution()
{
	return body->getRestitution();
}

const Vector3 &RigidBody::getTotalForce() const
{
	btVector3 btf = body->getTotalForce();
	return Vector3(double(btf.x()), double(btf.y()), double(btf.z()));
}

const Vector3& RigidBody::getTotalTorque() const
{
	btVector3 btt = body->getTotalTorque();
	return Vector3(double(btt.x()), double(btt.y()), double(btt.z()));
}

const Ogre::Quaternion& RigidBody::getOrientation() const
{
	btQuaternion btq = body->getOrientation();
	return {btq.w(), btq.x(), btq.y(), btq.z()};
}



const btVector3 RigidBody::parseToBulletVector(const Vector3& v) const
{
	return btVector3(btScalar(v.x), btScalar(v.y), btScalar(v.z));
}