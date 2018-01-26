#include "stdafx.h"
#include "steeringseek.h"
#include "character.h"
#include "params.h"

SteeringSeek::SteeringSeek()
{
	mVelocity = USVec2D(0.0f, 0.0f);
	mAcceleration = USVec2D(0.0f, 0.0f);
	mCharacterPosition = USVec2D(0.0f, 0.0f);
}

SteeringSeek::~SteeringSeek()
{

}


USVec2D SteeringSeek::GetSteering(Character* character, const USVec2D& target)
{
	mVelocity = target - character->GetLoc();

	if (mVelocity.Length() <= character->GetParams().dest_radius)
	{
		mVelocity = USVec2D(0.0f, 0.0f);
	}
	else
	{
		mVelocity.NormSafe();
		mVelocity *= character->GetParams().max_velocity;
	}

	mAcceleration = mVelocity - character->GetLinearVelocity();
	mAcceleration.NormSafe();
	mAcceleration *= character->GetParams().max_acceleration;
	
	return mAcceleration;
}

void SteeringSeek::DrawDebug(Character* character)
{
	mCharacterPosition = static_cast<USVec2D>(character->GetLoc());

	MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get();

	gfxDevice.SetPenColor(0.0f, 1.0f, 0.0f, 1.0f);
	MOAIDraw::DrawEllipseOutline(character->GetParams().targetPosition.mX, character->GetParams().targetPosition.mY, character->GetParams().dest_radius, character->GetParams().dest_radius, 50);

	gfxDevice.SetPenColor(0.0f, 0.0f, 1.0f, 1.0f);
	MOAIDraw::DrawLine(mCharacterPosition, mCharacterPosition + mVelocity);

	gfxDevice.SetPenColor(1.0f, 0.0f, 0.0f, 1.0f);
	MOAIDraw::DrawLine(mCharacterPosition, mCharacterPosition + mAcceleration);
}