#ifndef __NAVMESH_H__
#define __NAVMESH_H__

#include <vector>

struct Link;

struct Edge
{
	USVec2D mStart;
	USVec2D mEnd;

	float SquaredLegnth() { return (mEnd.mX - mStart.mX) * (mEnd.mX - mStart.mX) + (mEnd.mY - mStart.mY) * (mEnd.mY - mStart.mY); }
};

struct Polygon
{
	int mID;
	Polygon* mParent;

	int mG;
	int mH;

	std::vector<USVec2D> mVertices;
	std::vector<Edge> mEdges;

	std::vector<Polygon*> mNeighbours;
	std::vector<Link*> mLinks;

	USVec2D mCentroid;

	int GetF() const { return mG + mH; }
};

struct Link
{
	int mID;

	Polygon* mStartPolygon;
	Edge mStartEdge;

	Polygon* mEndPolygon;
	Edge mEndEdge;

	USVec2D mCenterPoint;
};

struct Navmesh
{
	std::vector<Polygon*> mPolygons;
	std::vector<Link*> mLinks;
};

bool ReadNavmesh(const char* filename, Navmesh& navmesh);
void FillPolygonEdges(Polygon& polygon);
void ComputePolygonCentroid(Polygon& polygon);
void ComputeLinkCenterPointAndFillNeighbours(Link& link);
bool IsLinkInPolygon(const Polygon* polygon, const Link* link);
USVec2D GetMiddlePoint(USVec2D pointA, USVec2D pointB);

#endif