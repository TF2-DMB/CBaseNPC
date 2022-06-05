#pragma once

#include "toolsnav.h"
#include "sourcesdk/nav_area.h"

#include <set>
#include <vector>
#include <cstdint>
#include <memory>
#include <unordered_map>

class CToolsNavMesh;
class CNavArea;
class CToolsNavArea
{
public:
	CNavArea* GetArea() const
	{
		return m_realArea;
	}

private:
	friend class CToolsNavMesh;
	CToolsNavArea(CNavArea* area);

	CNavArea* m_realArea;
};

class CToolsNavMesh
{
public:
	CToolsNavMesh();
	void OnCoreMapStart();
	void OnCoreMapEnd();

	void AddNavArea(CNavArea* area);

	CNavArea* GetNavArea(const Vector &pos, float beneathLimt = 120.0f) const;

	CNavArea* GetNavArea(CBaseEntity *pEntity, int nGetNavAreaFlags, float flBeneathLimit = 120.0f) const;

	CNavArea* GetNearestNavArea(const Vector &pos, bool anyZ, float maxDist, bool checkLOS, bool checkGround, int team) const;

	CNavArea* GetNavAreaByID(std::uint32_t id) const
	{
		return m_hashmap.at(id)->GetArea();
	}

	std::uint32_t GetNavAreaCount(void) const
	{
		return m_areas.size();
	}

	bool IsLoaded() const
	{
		return m_isLoaded;
	}

	template < typename Functor >
	bool ForAllAreasOverlappingExtent( Functor &func, const Extent &extent )
	{
		if (!m_grid.size())
		{
			return true;
		}

		static unsigned int searchMarker = RandomInt(0, 1024*1024);
		if (++searchMarker == 0)
		{
			++searchMarker;
		}

		Extent areaExtent;

		// get list in cell that contains position
		int startX = WorldToGridX( extent.lo.x );
		int endX = WorldToGridX( extent.hi.x );
		int startY = WorldToGridY( extent.lo.y );
		int endY = WorldToGridY( extent.hi.y );

		for( int x = startX; x <= endX; ++x )
		{
			for( int y = startY; y <= endY; ++y )
			{
				int iGrid = x + y * m_gridSizeX;
				if (iGrid >= (int)m_grid.size())
				{
					ExecuteNTimes(10, Warning( "** Walked off of the CNavMesh::m_grid in ForAllAreasOverlappingExtent()\n"));
					return true;
				}

				for (auto it = m_grid[iGrid].begin(); it != m_grid[iGrid].end(); it++)
				{
					CNavArea* area = (*it)->GetArea();
					// skip if we've already visited this area
					if (area->m_nearNavSearchMarker == searchMarker)
					{
						continue;
					}

					// mark as visited
					area->m_nearNavSearchMarker = searchMarker;
					area->GetExtent( &areaExtent );

					if (extent.IsOverlapping(areaExtent))
					{
						if (func( area ) == false)
						{
							return false;
						}
					}
				}
			}
		}
		return true;
	}
private:
	void AllocateGrid(float minX, float maxX, float minY, float maxY);
	std::int32_t WorldToGridX(float wx) const;
	std::int32_t WorldToGridY(float wy) const;

	std::set<std::unique_ptr<CToolsNavArea>> m_areas;
	std::vector<std::vector<CToolsNavArea*>> m_grid;
	float m_gridCellSize;
	std::int32_t m_gridSizeX;
	std::int32_t m_gridSizeY;
	float m_minX;
	float m_minY;
	bool m_isLoaded;
	std::uint32_t m_areaCount;
	std::unordered_map<std::uint32_t, CToolsNavArea*> m_hashmap;
};

extern CToolsNavMesh* ToolsNavMesh;

inline std::int32_t CToolsNavMesh::WorldToGridX(float wx) const
{ 
	std::int32_t x = (wx - m_minX) / m_gridCellSize;

	if (x < 0)
	{
		x = 0;
	}
	else if (x >= m_gridSizeX)
	{
		x = m_gridSizeX - 1;
	}
	
	return x;
}

inline std::int32_t CToolsNavMesh::WorldToGridY(float wy) const
{ 
	std::int32_t y = (wy - m_minY) / m_gridCellSize;

	if (y < 0)
	{
		y = 0;
	}
	else if (y >= m_gridSizeY)
	{
		y = m_gridSizeY - 1;
	}
	
	return y;
}