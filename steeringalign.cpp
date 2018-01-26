#include "stdafx.h"
#include "steeringalign.h"
#include "character.h"
#include "params.h"
#include <math.h>

SteeringAlign::SteeringAlign()
{
	mAngularVelocity = 0.0f;
	mAngularAcceleration = 0.0f;
}

SteeringAlign::~SteeringAlign()
{

}

float SteeringAlign::GetAngularSteering(Character* character, float targetRotation)
{
	mAngularVelocity = targetRotation - character->GetRot();

	// Constraint angle
	mAngularVelocity = ConstraintAngle(mAngularVelocity);

	if (abs(mAngularVelocity) <= character->GetParams().angular_arrive_radius)
	{
		mAngularVelocity = character->GetParams().max_angular_velocity * (mAngularVelocity / character->GetParams().angular_arrive_radius);
	}

	mAngularAcceleration = mAngularVelocity - character->GetAngularVelocity();

	// Clamp to max angular acceleration
	if (mAngularAcceleration < 0.0f)
	{
		mAngularAcceleration = -character->GetParams().max_angular_acceleration;
	}
	else
	{
		mAngularAcceleration = character->GetParams().max_angular_acceleration;
	}

	return mAngularAcceleration;
}

void SteeringAlign::DrawDebug(Character* character)
{

}

float SteeringAlign::ConstraintAngle(float angle)
{
	// Constraint to [-180, 180)
	angle = fmod(angle + 180, 360);

	if (angle < 0)
	{
		angle += 360;
	}

	return angle - 180;
}