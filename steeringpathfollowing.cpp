#include "stdafx.h"
#include "steeringpathfollowing.h"
#include "steeringseek.h"
#include "character.h"
#include "params.h"

SteeringPathFollowing::SteeringPathFollowing()
{
	mSeekSteering = new SteeringSeek();

	mVelocity = USVec2D(0.0f, 0.0f);
	mAcceleration = USVec2D(0.0f, 0.0f);

	mClosestPoint = USVec2D(0.0f, 0.0f);
	mLookAheadPoint = USVec2D(0.0f, 0.0f);
	mNextPointInPath = 0;
}

SteeringPathFollowing::~SteeringPathFollowing()
{
	delete mSeekSteering;
}

USVec2D SteeringPathFollowing::GetSteering(Character* character, const USVec2D& target)
{
	return mSeekSteering->GetSteering(character, GetAheadPoint(character));
}

USVec2D SteeringPathFollowing::GetAheadPoint(Character* character)
{
	Path path = character->GetPath();
	int numberOfPointsInPath = path.mPoints.size();

	if (numberOfPointsInPath > 1)
	{
		int firstNearIndex = 0;

		float distanceToCharacter = 0.0f;
		float bestDistance = 100000.0f;
		float dotProduct = 0.0f;

		USVec3D characterLocation = character->GetLoc();
		USVec2D bestPoint = USVec2D(0.0f, 0.0f);
		USVec2D segment = USVec2D(0.0f, 0.0f);
		USVec2D normalizedSegment = USVec2D(0.0f, 0.0f);
		USVec2D distanceFromPointToCharacter = USVec2D(0.0f, 0.0f);

		USVec2D nextSegment = USVec2D(0.0f, 0.0f);
		float remainder = 0.0f;

		for (int i = 0; i < numberOfPointsInPath - 1; ++i)
		{
			segment = path.mPoints[i + 1] - path.mPoints[i];

			normalizedSegment = segment;
			normalizedSegment.NormSafe();

			distanceFromPointToCharacter = characterLocation - path.mPoints[i];
			dotProduct = normalizedSegment.Dot(distanceFromPointToCharacter);
			dotProduct /= segment.Length();

			if (dotProduct >= 1.0f)
			{
				mClosestPoint = path.mPoints[i + 1];
			}
			else if (dotProduct <= 0.0f)
			{
				mClosestPoint = path.mPoints[i];
			}
			else
			{
				mClosestPoint = path.mPoints[i] + segment * dotProduct;
			}

			distanceToCharacter = (mClosestPoint - character->GetLoc()).LengthSquared();

			if (distanceToCharacter < bestDistance)
			{
				bestDistance = distanceToCharacter;
				bestPoint = mClosestPoint;
				mNextPointInPath = i + 1;
			}
		}

		mClosestPoint = bestPoint;
		segment = path.mPoints[mNextPointInPath] - path.mPoints[mNextPointInPath - 1];

		normalizedSegment = segment;
		normalizedSegment.NormSafe();

		// Look ahead point
		remainder = character->GetParams().look_ahead - (path.mPoints[mNextPointInPath] - mClosestPoint).Length();

		if (remainder > 0.0f)
		{
			int previousTargetPointInPath = mNextPointInPath;

			if (mNextPointInPath < (numberOfPointsInPath - 1))
			{
				for (; previousTargetPointInPath < numberOfPointsInPath - 1; ++previousTargetPointInPath)
				{
					float aux = remainder - (path.mPoints[previousTargetPointInPath + 1] - path.mPoints[previousTargetPointInPath]).Length();
					
					if (aux > 0.0f)
					{
						remainder -= aux;
					}
					else
					{
						break;
					}
				}

				if (path.mPoints.size() > previousTargetPointInPath + 1)
				{
					nextSegment = path.mPoints[previousTargetPointInPath + 1] - path.mPoints[previousTargetPointInPath];
				}
			}
			else
			{
				nextSegment = path.mPoints[mNextPointInPath] - path.mPoints[mNextPointInPath - 1];
			}

			USVec2D normalizedNextSegment = nextSegment;
			normalizedNextSegment.NormSafe();

			mLookAheadPoint = path.mPoints[previousTargetPointInPath] + normalizedNextSegment * remainder;
		}
		else
		{
			mLookAheadPoint = mClosestPoint + normalizedSegment * character->GetParams().look_ahead;
		}

		return mLookAheadPoint;
	}
	else if (numberOfPointsInPath == 1)
	{
		return path.mPoints[0];
	}
	
	return character->GetLoc();
}

void SteeringPathFollowing::DrawDebug(Character* character)
{
	MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get();

	// Look ahead point
	gfxDevice.SetPenColor(0.7f, 0.0f, 0.0f, 1.0f);
	MOAIDraw::DrawEllipseFill(mLookAheadPoint.mX, mLookAheadPoint.mY, 5, 5, 500);

	// Closest point
	gfxDevice.SetPenColor(0.0f, 0.7f, 0.0f, 1.0f);
	MOAIDraw::DrawEllipseFill(mClosestPoint.mX, mClosestPoint.mY, 5, 5, 500);

	// Next point
	gfxDevice.SetPenColor(0.0f, 0.7f, 0.6f, 1.0f);
	MOAIDraw::DrawEllipseFill(character->GetPath().mPoints[mNextPointInPath].mX, character->GetPath().mPoints[mNextPointInPath].mY, 5, 5, 500);

	int numberOfPointsInPath = character->GetPath().mPoints.size();

	if (numberOfPointsInPath > 0)
	{
		// Outline each point in path
		gfxDevice.SetPenColor(0.7f, 0.6f, 0.0f, 1.0f);

		for (int i = 0; i < numberOfPointsInPath; ++i)
		{
			MOAIDraw::DrawEllipseOutline(character->GetPath().mPoints[i].mX, character->GetPath().mPoints[i].mY, character->GetParams().dest_radius, character->GetParams().dest_radius, 500);
		}

		// Line between points
		gfxDevice.SetPenColor(0.7f, 0.0f, 0.6f, 1.0f);

		for (int i = 0; i < numberOfPointsInPath - 1; ++i)
		{
			MOAIDraw::DrawLine(character->GetPath().mPoints[i], character->GetPath().mPoints[i + 1]);
		}
	}
}