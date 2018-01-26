#ifndef __STEERINGSEEK_H__
#define __STEERINGSEEK_H__

#include "steering.h"

class SteeringSeek : public Steering
{
public:
	SteeringSeek();
	~SteeringSeek();

	virtual USVec2D GetSteering(Character* character, const USVec2D& target);
	virtual float GetAngularSteering(Character* character, float targetRotation) { return 0.0f; }
	virtual void DrawDebug(Character* character);

private:
	USVec2D mVelocity;
	USVec2D mAcceleration;

	USVec2D mCharacterPosition;
};

#endif