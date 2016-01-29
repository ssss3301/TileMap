#pragma once

#include <vector>
#include <set>
#include <utility>
#include "TileData.h"

class RouteAlgorithm
{
public:
	RouteAlgorithm(bool allowDiagonal = false);
	RouteAlgorithm(int width, int height, const std::vector<TileData*> mapTileDatas, bool allowDiagonal = false);
	~RouteAlgorithm();

public:
	void setAllowDiagonal(bool allowDiagonal);
	bool allowDiagonal() const;
	void setAllowCrossConrner(bool allowCrossConrner);
	bool allowCrossConrner() const;
	void setMapSize(int width, int height);
	void setTileDatas(std::vector<TileData*> mapTileDatas);
	void findingPath(TileData* startTile, TileData* endTile);
	std::vector<TileData*> path() const;

private:
	TileData* getTileDataByPosition(const std::pair<float, float>& position);
	void Neighbor(const TileData* tile, std::vector<TileData*>* neighbors);
	int computeHScore(const TileData* tile, const TileData* endTile);
	void insertToOpenList(TileData* tile);
	std::pair<float, float> String2Pair(const std::string* _str);
	int moveCost(const TileData* originTile, const TileData* destinationTile);
	void constructPath(const TileData* endTile);
	
private:
	std::string createStringWithFormat(const char* format, ...);
//	std::set<TileData*> nearestNeighborWalkAbleTile(TileData* tile);

private:
	int						m_mapWidth;
	int						m_mapHeight;
	bool					m_allowDiagonal;//是否允许对角线走动
	bool					m_allowCrossCorner;
	std::vector<TileData*>	m_tileDatas;
	std::vector<TileData*>	m_openList;
	std::vector<TileData*>	m_closeList;
	std::vector<TileData*>	m_route;
	std::vector<TileData*>	m_neighborCloseList;
};