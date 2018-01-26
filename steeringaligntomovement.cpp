#include "stdafx.h"
#include "steeringaligntomovement.h"
#include "steeringalign.h"
#include "character.h"
#include "params.h"
#include <math.h>

SteeringAlignToMovement::SteeringAlignToMovement()
{
	mAlignSteering = new SteeringAlign();

	mAngularVelocity = 0.0f;
	mAngularAcceleration = 0.0f;
	mAngle = 0.0f;
	mRadiansToDegrees = 57.2957f;
}

SteeringAlignToMovement::~SteeringAlignToMovement()
{
	delete mAlignSteering;
}

float SteeringAlignToMovement::GetAngularSteering(Character* character, float targetRotation)
{
	// Calculate angle
	mAngle = atan2(character->GetLinearVelocity().mY, character->GetLinearVelocity().mX) * mRadiansToDegrees;
	
	if (mAngle < 0.0f)
	{
		mAngle += 360.0f;
	}

	// Apply align steering
	mAngularAcceleration = mAlignSteering->GetAngularSteering(character, mAngle);

	return mAngularAcceleration;
}

void SteeringAlignToMovement::DrawDebug(Character* character)
{

}