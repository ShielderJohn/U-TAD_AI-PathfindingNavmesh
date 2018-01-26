#ifndef __STEERINGALIGN_H__
#define __STEERINGALIGN_H__

#include "steering.h"

class SteeringAlign : public Steering
{
public:
	SteeringAlign();
	~SteeringAlign();

	virtual USVec2D GetSteering(Character* character, const USVec2D& target) { return{ 0.0f, 0.0f }; }
	virtual float GetAngularSteering(Character* character, float targetRotation);
	virtual void DrawDebug(Character* character);

private:
	float mAngularVelocity;
	float mAngularAcceleration;

	float ConstraintAngle(float angle);
};

#endif