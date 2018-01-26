#ifndef __PATHFINDER_H__
#define __PATHFINDER_H__

#include <moaicore/MOAIEntity2D.h>
#include "navmesh.h"
#include "path.h"

class Pathfinder: public virtual MOAIEntity2D
{
public:
	Pathfinder();
	~Pathfinder();

	virtual void DrawDebug();

	void SetStartPosition(float x, float y);
	void SetEndPosition(float x, float y);
	const USVec2D& GetStartPosition() const { return m_StartPosition;}
	const USVec2D& GetEndPosition() const { return m_EndPosition;}

	Pathfinder* GetPointerToThis() { return this; }

    bool PathfindStep();

	void TracePath(const Polygon* polygon);
	Path GetPath() const { return mPath; }
private:
	void UpdatePath();

	Polygon* GetPolygonFromPosition(USVec2D position);
	Polygon* PopPolygonWithLowestCost(std::vector<Polygon*>& list);
	bool IsPolygonInList(const Polygon* polygon, const std::vector<Polygon*>& list);
	int GetDistanceBetweenPolygons(const Polygon* polygonA, const Polygon* polygonB);

	void Reset();
private:
	USVec2D m_StartPosition;
	USVec2D m_EndPosition;

	Polygon* mStartPolygon;
	Polygon* mTargetPolygon;

	Navmesh mNavmesh;
	Path mPath;

	std::vector<Polygon*> mOpenList;
	std::vector<Polygon*> mClosedList;

	// Lua configuration
public:
	DECL_LUA_FACTORY(Pathfinder)
public:
	virtual void RegisterLuaFuncs(MOAILuaState& state);
private:
	static int _setStartPosition(lua_State* L);
	static int _setEndPosition(lua_State* L);
    static int _pathfindStep(lua_State* L);
	static int _getPointerToThis(lua_State* L);
};


#endif