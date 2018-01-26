#ifndef __STEERINGPATHFOLLOWING_H__
#define __STEERINGPATHFOLLOWING_H__

#include "steering.h"

class SteeringSeek;

class SteeringPathFollowing : public Steering
{
public:
	SteeringPathFollowing();
	~SteeringPathFollowing();

	virtual USVec2D GetSteering(Character* character, const USVec2D& target);
	virtual float GetAngularSteering(Character* character, float targetRotation) { return 0.0f; }
	virtual void DrawDebug(Character* character);

private:
	SteeringSeek* mSeekSteering;

	USVec2D mVelocity;
	USVec2D mAcceleration;

	USVec2D mClosestPoint;
	USVec2D mLookAheadPoint;
	int mNextPointInPath;

	USVec2D GetAheadPoint(Character* character);
};

#endif