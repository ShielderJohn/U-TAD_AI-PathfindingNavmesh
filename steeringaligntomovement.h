#ifndef __STEERINGALIGNTOMOVEMENT_H__
#define __STEERINGALIGNTOMOVEMENT_H__

#include "steering.h"

class SteeringAlign;

class SteeringAlignToMovement : public Steering
{
public:
	SteeringAlignToMovement();
	~SteeringAlignToMovement();

	virtual USVec2D GetSteering(Character* character, const USVec2D& target) { return{ 0.0f, 0.0f }; }
	virtual float GetAngularSteering(Character* character, float targetRotation);
	virtual void DrawDebug(Character* character);

private:
	SteeringAlign* mAlignSteering;

	float mAngularVelocity;
	float mAngularAcceleration;
	float mAngle;
	float mRadiansToDegrees;
};

#endif