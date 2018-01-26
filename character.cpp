#include <stdafx.h>
#include "character.h"
#include <tinyxml.h>

#include "steeringaligntomovement.h"
#include "steeringpathfollowing.h"
#include "pathfinding/pathfinder.h"

Character::Character() : mLinearVelocity(0.0f, 0.0f), mAngularVelocity(0.0f)
{
	RTTI_BEGIN
		RTTI_EXTEND(MOAIEntity2D)
		RTTI_END

	mPathfinder = nullptr;

	mSteerings.push_back(new SteeringAlignToMovement());
	mSteerings.push_back(new SteeringPathFollowing());
}

Character::~Character()
{

}

void Character::OnStart()
{
	ReadParams("params.xml", mParams);
}

void Character::OnStop()
{

}

void Character::OnUpdate(float step)
{
	if (mPathfinder != nullptr)
	{
		mPath = mPathfinder->GetPath();
	}
	
	if (mPath.mPoints.size() > 0)
	{
		USVec2D linearAcceleration = USVec2D(0.0f, 0.0f);
		float angularAcceleraton = 0.0f;

		std::vector<USVec2D> linearAccelerations;
		std::vector<float> angularAccelerations;

		float linearAccelerationTotalWeight = 0.0f;
		float angularAccelerationTotalWeight = 0.0f;

		int numberOfSteerings = mSteerings.size();

		for (int i = 0; i < numberOfSteerings; ++i)
		{
			linearAcceleration += mSteerings[i]->GetSteering(this, mParams.targetPosition);
			angularAcceleraton += mSteerings[i]->GetAngularSteering(this, mParams.targetRotation);
		}

		mLinearVelocity += linearAcceleration * step;
		mAngularVelocity += angularAcceleraton * step;

		// Clamp to max velocity
		if (mLinearVelocity.Length() > mParams.max_velocity)
		{
			mLinearVelocity.NormSafe();
			mLinearVelocity *= mParams.max_velocity;
		}

		SetLoc(GetLoc() + mLinearVelocity * step);

		// Clamp to max angular velocity
		if (abs(mAngularVelocity) > mParams.max_angular_acceleration)
		{
			if (mAngularVelocity > 0.0f)
			{
				mAngularVelocity = mParams.max_angular_velocity;
			}
			else if (mAngularVelocity < 0.0f)
			{
				mAngularVelocity = -mParams.max_angular_velocity;
			}
		}

		SetRot(GetRot() + mAngularVelocity * step);
	}
}

void Character::DrawDebug()
{
	MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get();
	
	if (mPath.mPoints.size() > 0)
	{
		// Orientation
		gfxDevice.SetPenColor(1.0f, 1.0f, 1.0f, 1.0f);
		MOAIDraw::DrawLine(static_cast<USVec2D>(GetLoc()), static_cast<USVec2D>(GetLoc()) + mLinearVelocity);

		int numberOfSteerings = mSteerings.size();

		for (int i = 0; i < numberOfSteerings; ++i)
		{
			mSteerings[i]->DrawDebug(this);
		}
	}
}



// Lua configuration

void Character::RegisterLuaFuncs(MOAILuaState& state)
{
	MOAIEntity2D::RegisterLuaFuncs(state);
	
	luaL_Reg regTable [] = {
		{ "setLinearVel",			_setLinearVel},
		{ "setAngularVel",			_setAngularVel},
		{ "setPathfinder",			_setPathfinder },
		{ NULL, NULL }
	};

	luaL_register(state, 0, regTable);
}

int Character::_setLinearVel(lua_State* L)
{
	MOAI_LUA_SETUP(Character, "U")
	
	float pX = state.GetValue<float>(2, 0.0f);
	float pY = state.GetValue<float>(3, 0.0f);
	self->SetLinearVelocity(pX, pY);
	return 0;	
}

int Character::_setAngularVel(lua_State* L)
{
	MOAI_LUA_SETUP(Character, "U")
	
	float angle = state.GetValue<float>(2, 0.0f);
	self->SetAngularVelocity(angle);

	return 0;
}

int Character::_setPathfinder(lua_State* L)
{
	MOAI_LUA_SETUP(Character, "U")

	void* ptr = lua_touserdata(state, 2);

	self->SetPathfinder(reinterpret_cast<Pathfinder*>(ptr));

	return 0;
}
	