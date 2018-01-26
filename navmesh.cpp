#include "stdafx.h"
#include "tinyxml.h"
#include "navmesh.h"

bool ReadNavmesh(const char* filename, Navmesh& navmesh)
{
	TiXmlDocument doc(filename);
	if (!doc.LoadFile())
	{
		fprintf(stderr, "Couldn't read path from %s", filename);
		return false;
	}

	TiXmlHandle hDoc(&doc);

	TiXmlElement* pElem;
	pElem = hDoc.FirstChildElement().Element();
	if (!pElem)
	{
		fprintf(stderr, "Invalid format for %s", filename);
		return false;
	}

	// Polygons
	TiXmlHandle hRoot(pElem);
	TiXmlHandle hPolygons = hRoot.FirstChildElement("polygons");

	TiXmlElement* polygonElem = hPolygons.FirstChild().Element();
	int polygonID = 0;

	for (polygonElem; polygonElem; polygonElem = polygonElem->NextSiblingElement())
	{
		TiXmlElement* pointElem = polygonElem->FirstChildElement();

		Polygon* polygon = new Polygon();
		USVec2D point;

		for (pointElem; pointElem; pointElem = pointElem->NextSiblingElement())
		{
			pointElem->Attribute("x", &point.mX);
			pointElem->Attribute("y", &point.mY);

			polygon->mVertices.push_back(point);
		}

		polygon->mID = polygonID;
		polygon->mG = 0;
		polygon->mH = 0;
		polygon->mParent = nullptr;

		FillPolygonEdges(*polygon);
		ComputePolygonCentroid(*polygon);

		navmesh.mPolygons.push_back(polygon);

		polygonID += 1;
	}

	// Links
	TiXmlHandle hLinks = hRoot.FirstChildElement("links");

	TiXmlElement* linkElem = hLinks.FirstChild().Element();
	TiXmlElement* linkStartElem;
	TiXmlElement* linkEndElem;
	int linkID = 0;

	for (linkElem; linkElem; linkElem = linkElem->NextSiblingElement())
	{
		Link* link = new Link();

		linkStartElem = linkElem->FirstChildElement("start");
		linkEndElem = linkElem->FirstChildElement("end");

		int startPolygon;
		int startPolygonEdgeStart;
		int startPolygonEdgeEnd;

		int endPolygon;
		int endPolygonEdgeStart;
		int endPolygonEdgeEnd;

		linkStartElem->Attribute("polygon", &startPolygon);
		linkStartElem->Attribute("edgestart", &startPolygonEdgeStart);
		linkStartElem->Attribute("edgeend", &startPolygonEdgeEnd);

		linkEndElem->Attribute("polygon", &endPolygon);
		linkEndElem->Attribute("edgestart", &endPolygonEdgeStart);
		linkEndElem->Attribute("edgeend", &endPolygonEdgeEnd);

		link->mStartPolygon = navmesh.mPolygons[startPolygon];
		link->mStartEdge.mStart = link->mStartPolygon->mVertices[startPolygonEdgeStart];
		link->mStartEdge.mEnd = link->mStartPolygon->mVertices[startPolygonEdgeEnd];

		link->mEndPolygon = navmesh.mPolygons[endPolygon];
		link->mEndEdge.mStart = link->mEndPolygon->mVertices[endPolygonEdgeStart];
		link->mEndEdge.mEnd = link->mEndPolygon->mVertices[endPolygonEdgeEnd];

		link->mID = linkID;

		ComputeLinkCenterPointAndFillNeighbours(*link);
		navmesh.mLinks.push_back(link);

		linkID += 1;
	}

	return true;
}

void FillPolygonEdges(Polygon& polygon)
{
	int numberOfPoints = polygon.mVertices.size();
	Edge edge;

	for (int i = 0; i < numberOfPoints; ++i)
	{
		if (i == numberOfPoints - 1)
		{
			edge.mStart = { polygon.mVertices[i].mX, polygon.mVertices[i].mY };
			edge.mEnd = { polygon.mVertices[0].mX, polygon.mVertices[0].mY };

			polygon.mEdges.push_back(edge);
		}
		else
		{
			edge.mStart = { polygon.mVertices[i].mX, polygon.mVertices[i].mY };
			edge.mEnd = { polygon.mVertices[i + 1].mX, polygon.mVertices[i + 1].mY };

			polygon.mEdges.push_back(edge);
		}
	}
}

void ComputePolygonCentroid(Polygon& polygon)
{
	USVec2D centroid = USVec2D(0.0f, 0.0f);

	float signedArea = 0.0f;
	float partialSignedArea = 0.0f;

	float currentVertexX = 0.0f;
	float currentVertexY = 0.0f;

	float nextVertexX = 0.0f;
	float nextVertexY = 0.0f;

	int numberOfVertices = polygon.mVertices.size();

	int i = 0;

	for (i = 0; i < numberOfVertices - 1; ++i)
	{
		currentVertexX = polygon.mVertices[i].mX;
		currentVertexY = polygon.mVertices[i].mY;

		nextVertexX = polygon.mVertices[i + 1].mX;
		nextVertexY = polygon.mVertices[i + 1].mY;

		partialSignedArea = currentVertexX * nextVertexY - nextVertexX * currentVertexY;
		signedArea += partialSignedArea;

		centroid.mX += (currentVertexX + nextVertexX) * partialSignedArea;
		centroid.mY += (currentVertexY + nextVertexY) * partialSignedArea;
	}

	currentVertexX = polygon.mVertices[i].mX;
	currentVertexY = polygon.mVertices[i].mY;

	nextVertexX = polygon.mVertices[0].mX;
	nextVertexY = polygon.mVertices[0].mY;

	partialSignedArea = currentVertexX * nextVertexY - nextVertexX * currentVertexY;;
	signedArea += partialSignedArea;

	centroid.mX += (currentVertexX + nextVertexX) * partialSignedArea;
	centroid.mY += (currentVertexY + nextVertexY) * partialSignedArea;

	signedArea *= 0.5f;
	centroid.mX /= (6.0f * signedArea);
	centroid.mY /= (6.0f * signedArea);

	polygon.mCentroid = centroid;
}

void ComputeLinkCenterPointAndFillNeighbours(Link& link)
{
	float startEdgeSquaredLength = 0.0f;
	float endEdgeSquaredLength = 0.0f;

	// Center point
	startEdgeSquaredLength = link.mStartEdge.SquaredLegnth();
	endEdgeSquaredLength = link.mEndEdge.SquaredLegnth();

	if (startEdgeSquaredLength < endEdgeSquaredLength)
	{
		link.mCenterPoint = GetMiddlePoint(link.mStartEdge.mStart, link.mStartEdge.mEnd);
	}
	else
	{
		link.mCenterPoint = GetMiddlePoint(link.mEndEdge.mStart, link.mEndEdge.mEnd);
	}

	// Neighbours
	link.mStartPolygon->mNeighbours.push_back(link.mEndPolygon);
	
	if (!IsLinkInPolygon(link.mStartPolygon, &link))
	{
		link.mStartPolygon->mLinks.push_back(&link);
	}

	link.mEndPolygon->mNeighbours.push_back(link.mStartPolygon);

	if (!IsLinkInPolygon(link.mEndPolygon, &link))
	{
		link.mEndPolygon->mLinks.push_back(&link);
	}
}

bool IsLinkInPolygon(const Polygon* polygon, const Link* link)
{
	int numberOfLinks = polygon->mLinks.size();

	for (int i = 0; i < numberOfLinks; ++i)
	{
		if (polygon->mLinks[i]->mID == link->mID)
		{
			return true;
		}
	}

	return false;
}

USVec2D GetMiddlePoint(USVec2D pointA, USVec2D pointB)
{
	return { (pointA.mX + pointB.mX) / 2, (pointA.mY + pointB.mY) / 2 };
}