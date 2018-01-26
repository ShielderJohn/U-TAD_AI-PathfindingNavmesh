#include <stdafx.h>

#include "pathfinder.h"

Pathfinder::Pathfinder() : MOAIEntity2D()
{
	RTTI_BEGIN
		RTTI_EXTEND(MOAIEntity2D)
	RTTI_END

	ReadNavmesh("navmesh.xml", mNavmesh);

	mStartPolygon = nullptr;
	mTargetPolygon = nullptr;
}

Pathfinder::~Pathfinder()
{

}

void Pathfinder::SetStartPosition(float x, float y)
{
	mStartPolygon = GetPolygonFromPosition({ x, y });

	if (mStartPolygon != nullptr)
	{
		m_StartPosition = USVec2D(x, y);
		UpdatePath();
	}
}

void Pathfinder::SetEndPosition(float x, float y)
{
	mTargetPolygon = GetPolygonFromPosition({ x, y });

	if (mTargetPolygon != nullptr)
	{
		m_EndPosition = USVec2D(x, y);
		UpdatePath();
	}
}

void Pathfinder::UpdatePath()
{
	Reset();

	if (mStartPolygon == nullptr || mTargetPolygon == nullptr)
	{
		return;
	}

	mOpenList.push_back(mStartPolygon);

	while (mOpenList.size() > 0)
	{
		Polygon* currentPolygon = PopPolygonWithLowestCost(mOpenList);

		if (currentPolygon != nullptr)
		{
			mClosedList.push_back(currentPolygon);

			if (currentPolygon->mID == mTargetPolygon->mID)
			{
				TracePath(currentPolygon);
				return;
			}

			int numberOfNeighbours = currentPolygon->mNeighbours.size();

			for (int i = 0; i < numberOfNeighbours; ++i)
			{
				if (IsPolygonInList(currentPolygon->mNeighbours[i], mClosedList))
				{
					continue;
				}

				int costToNeighbour = currentPolygon->mG + GetDistanceBetweenPolygons(currentPolygon, currentPolygon->mNeighbours[i]);

				if (costToNeighbour < currentPolygon->mNeighbours[i]->mG || !IsPolygonInList(currentPolygon->mNeighbours[i], mOpenList))
				{
					currentPolygon->mNeighbours[i]->mG = costToNeighbour;
					currentPolygon->mNeighbours[i]->mH = GetDistanceBetweenPolygons(currentPolygon->mNeighbours[i], mTargetPolygon);
					currentPolygon->mNeighbours[i]->mParent = currentPolygon;

					if (!IsPolygonInList(currentPolygon->mNeighbours[i], mOpenList))
					{
						mOpenList.push_back(currentPolygon->mNeighbours[i]);
					}
				}
			}
		}
	}
}

void Pathfinder::DrawDebug()
{
	MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get();

	int numberOfPolygons = mNavmesh.mPolygons.size();
	int numberOfLinks = mNavmesh.mLinks.size();

	// Polygons
	for (int i = 0; i < numberOfPolygons; ++i)
	{
		gfxDevice.SetPenColor(0.15f, 0.0f, 0.55f, 0.5f);
		MOAIDraw::DrawPolygonFilled(mNavmesh.mPolygons[i]->mVertices);

		gfxDevice.SetPenColor(0.75f, 0.35f, 0.65f, 1.0f);
		MOAIDraw::DrawPolygon(mNavmesh.mPolygons[i]->mVertices);

		// Centroid
		/*gfxDevice.SetPenColor(1.0f, 0.0f, 0.0f, 1.0f);
		MOAIDraw::DrawEllipseFill(mNavmesh.mPolygons[i]->mCentroid.mX, mNavmesh.mPolygons[i]->mCentroid.mY, 4, 4, 50);*/
	}

	// Links
	/*for (int i = 0; i < numberOfLinks; ++i)
	{
		gfxDevice.SetPenColor(0.0f, 0.6f, 0.0f, 1.0f);
		MOAIDraw::DrawEllipseFill(mNavmesh.mLinks[i]->mCenterPoint.mX, mNavmesh.mLinks[i]->mCenterPoint.mY, 8, 8, 50);
	}*/

	// Start and target position
	gfxDevice.SetPenColor(0.8f, 0.75f, 0.0f, 1.0f);
	MOAIDraw::DrawEllipseFill(m_StartPosition.mX, m_StartPosition.mY, 6, 6, 50);

	gfxDevice.SetPenColor(0.0f, 0.75f, 0.8f, 1.0f);
	MOAIDraw::DrawEllipseFill(m_EndPosition.mX, m_EndPosition.mY, 6, 6, 50);

	// Path
	int pathSize = mPath.mPoints.size();

	if (pathSize > 0)
	{
		for (int i = 0; i < pathSize; ++i)
		{
			if (i > 0)
			{
				gfxDevice.SetPenColor(0.85f, 0.15f, 0.15f, 1.0f);
				MOAIDraw::DrawLine(mPath.mPoints[i - 1], mPath.mPoints[i]);
			}
		}
	}
}

bool Pathfinder::PathfindStep()
{
    // returns true if pathfinding process finished
    return true;
}

void Pathfinder::TracePath(const Polygon* polygon)
{
	if (polygon != nullptr)
	{
		const Polygon* pathPolygon = polygon;
		mPath.mPoints.push_back(m_EndPosition);

		while (pathPolygon->mParent != nullptr)
		{
			int numberOfLinks = pathPolygon->mLinks.size();

			for (int i = 0; i < numberOfLinks; ++i)
			{
				if (pathPolygon->mLinks[i]->mStartPolygon->mID == pathPolygon->mParent->mID ||
					pathPolygon->mLinks[i]->mEndPolygon->mID == pathPolygon->mParent->mID)
				{
					mPath.mPoints.insert(mPath.mPoints.begin(), pathPolygon->mLinks[i]->mCenterPoint);
				}
			}

			pathPolygon = pathPolygon->mParent;
		}

		mPath.mPoints.insert(mPath.mPoints.begin(), m_StartPosition);
	}
}


Polygon* Pathfinder::GetPolygonFromPosition(USVec2D position)
{
	int numberOfPolygons = mNavmesh.mPolygons.size();
	int numberOfPoints = 0;

	// Get max and min point values
	for (int i = 0; i < numberOfPolygons; ++i)
	{
		numberOfPoints = mNavmesh.mPolygons[i]->mVertices.size();

		float minX = mNavmesh.mPolygons[i]->mVertices[0].mX;
		float maxX = mNavmesh.mPolygons[i]->mVertices[0].mX;
		float minY = mNavmesh.mPolygons[i]->mVertices[0].mY;
		float maxY = mNavmesh.mPolygons[i]->mVertices[0].mY;

		for (int j = 0; j < numberOfPoints; ++j)
		{
			if (mNavmesh.mPolygons[i]->mVertices[j].mX < minX)
			{
				minX = mNavmesh.mPolygons[i]->mVertices[j].mX;
			}
			else if (mNavmesh.mPolygons[i]->mVertices[j].mX > maxX)
			{
				maxX = mNavmesh.mPolygons[i]->mVertices[j].mX;
			}

			if (mNavmesh.mPolygons[i]->mVertices[j].mY < minY)
			{
				minY = mNavmesh.mPolygons[i]->mVertices[j].mY;
			}
			else if (mNavmesh.mPolygons[i]->mVertices[j].mY > maxY)
			{
				maxY = mNavmesh.mPolygons[i]->mVertices[j].mY;
			}
		}

		if (position.mX >= minX && position.mX <= maxX && position.mY >= minY && position.mY <= maxY)
		{
			return mNavmesh.mPolygons[i];
		}
	}

	return nullptr;
}

Polygon* Pathfinder::PopPolygonWithLowestCost(std::vector<Polygon*>& list)
{
	int mOpenListSize = list.size();

	if (mOpenListSize > 0)
	{
		Polygon* polygonToReturn = list[0];
		int indexOfNodeWithLowestCost = 0;

		for (int i = 0; i < mOpenListSize; ++i)
		{
			if (list[i]->GetF() < polygonToReturn->GetF())
			{
				polygonToReturn = list[i];
				indexOfNodeWithLowestCost = i;
			}
		}

		list.erase(list.begin() + indexOfNodeWithLowestCost);

		return polygonToReturn;
	}

	return nullptr;
}

bool Pathfinder::IsPolygonInList(const Polygon* polygon, const std::vector<Polygon*>& list)
{
	int listSize = list.size();

	for (int i = 0; i < listSize; ++i)
	{
		if (list[i]->mID == polygon->mID)
		{
			return true;
		}
	}

	return false;
}

int Pathfinder::GetDistanceBetweenPolygons(const Polygon* polygonA, const Polygon* polygonB)
{
	int distanceX = abs(polygonA->mCentroid.mX - polygonB->mCentroid.mX);
	int distanceY = abs(polygonA->mCentroid.mY - polygonB->mCentroid.mY);

	if (distanceX > distanceY)
	{
		return 14 * distanceY + 10 * (distanceX - distanceY);
	}

	return 14 * distanceX + 10 * (distanceY - distanceX);
}

void Pathfinder::Reset()
{
	mPath.mPoints.clear();
	mOpenList.clear();
	mClosedList.clear();

	int numberOfPolygons = mNavmesh.mPolygons.size();

	for (int i = 0; i < numberOfPolygons; ++i)
	{
		mNavmesh.mPolygons[i]->mG = 0;
		mNavmesh.mPolygons[i]->mH = 0;
		mNavmesh.mPolygons[i]->mParent = nullptr;
	}
}












//lua configuration ----------------------------------------------------------------//
void Pathfinder::RegisterLuaFuncs(MOAILuaState& state)
{
	MOAIEntity::RegisterLuaFuncs(state);

	luaL_Reg regTable [] = {
		{ "setStartPosition",		_setStartPosition},
		{ "setEndPosition",			_setEndPosition},
        { "pathfindStep",           _pathfindStep},
		{ "getPointerToThis",   _getPointerToThis },
		{ NULL, NULL }
	};

	luaL_register(state, 0, regTable);
}

int Pathfinder::_setStartPosition(lua_State* L)
{
	MOAI_LUA_SETUP(Pathfinder, "U")

	float pX = state.GetValue<float>(2, 0.0f);
	float pY = state.GetValue<float>(3, 0.0f);
	self->SetStartPosition(pX, pY);
	return 0;
}

int Pathfinder::_setEndPosition(lua_State* L)
{
	MOAI_LUA_SETUP(Pathfinder, "U")

	float pX = state.GetValue<float>(2, 0.0f);
	float pY = state.GetValue<float>(3, 0.0f);
	self->SetEndPosition(pX, pY);
	return 0;
}

int Pathfinder::_pathfindStep(lua_State* L)
{
    MOAI_LUA_SETUP(Pathfinder, "U")

    self->PathfindStep();
    return 0;
}

int Pathfinder::_getPointerToThis(lua_State* L)
{
	MOAI_LUA_SETUP(Pathfinder, "U")

	lua_pushlightuserdata(state, self->GetPointerToThis());

	return 1;
}