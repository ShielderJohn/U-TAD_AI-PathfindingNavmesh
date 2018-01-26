#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include <moaicore/MOAIEntity2D.h>
#include <vector>

#include <params.h>
#include "path.h"

class Steering;
class Pathfinder;

class Character: public MOAIEntity2D
{
public:
    DECL_LUA_FACTORY(Character)
protected:
	virtual void OnStart();
	virtual void OnStop();
	virtual void OnUpdate(float step);
public:
	virtual void DrawDebug();

	Character();
	~Character();

	void SetPathfinder(Pathfinder* pathfinder) { mPathfinder = pathfinder; }
	
	void SetLinearVelocity(float x, float y) { mLinearVelocity.mX = x; mLinearVelocity.mY = y;}
	void SetAngularVelocity(float angle) { mAngularVelocity = angle;}
	
	USVec2D GetLinearVelocity() const { return mLinearVelocity;}
	float GetAngularVelocity() const { return mAngularVelocity;}

	Params GetParams() { return mParams; }
	Path GetPath() { return mPath; }
private:
	USVec2D mLinearVelocity;
	float mAngularVelocity;

	Pathfinder* mPathfinder;

	std::vector<Steering*> mSteerings;

	Params mParams;
	Path mPath;
	
	// Lua configuration
public:
	virtual void RegisterLuaFuncs(MOAILuaState& state);
private:
	static int _setLinearVel(lua_State* L);
	static int _setAngularVel(lua_State* L);
	static int _setPathfinder(lua_State *L);
};

#endif