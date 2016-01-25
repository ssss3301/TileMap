#include "RouteAlgorithm.h"

RouteAlgorithm::RouteAlgorithm(bool diagonalRule) :m_diagonalRule(diagonalRule)
{
	m_openList.clear();
	m_closeList.clear();
}

RouteAlgorithm::RouteAlgorithm(int width, int height, const std::vector<TileData*> mapTileDatas
	, bool diagonalRule) : m_mapWidth(width), m_mapHeight(height), m_tileDatas(mapTileDatas)
						, m_diagonalRule(diagonalRule)
{
	m_openList.clear();
	m_closeList.clear();
}

RouteAlgorithm::~RouteAlgorithm()
{

}

void RouteAlgorithm::setMapSize(int width, int height)
{
	m_mapWidth = width;
	m_mapHeight = height;
}

void RouteAlgorithm::setTileDatas(std::vector<TileData*> mapTileDatas)
{
	m_tileDatas = mapTileDatas;
}

void RouteAlgorithm::findingPath(TileData* startTile, TileData* endTile)
{
	m_neighborCloseList.clear();

	std::set<TileData*> nearestNighbor;
	if (!endTile->walkAble())
	{
		nearestNighbor = nearestNeighborWalkAbleTile(endTile);
	}

	std::vector<std::vector<TileData*>> paths;

	std::set<TileData*>::const_iterator iter = nearestNighbor.begin();
	while (iter != nearestNighbor.end())
	{
		GetRoute(startTile, *iter);
		paths.push_back(m_route);
		++iter;
	}

	m_route = paths[0];
	for (int index = 1; index < paths.size(); ++index)
	{
		if (m_route.size() > paths[index].size())
			m_route = paths[index];
	}
}

TileData* RouteAlgorithm::getTileDataByPosition(const std::pair<float, float>& position)
{
	for each (TileData* tile in m_tileDatas)
	{
		if (tile)
		{
			if (tile->position() == position)
				return tile;
		}
	}

	return nullptr;
}

void RouteAlgorithm::Neighbor(const TileData* tile, std::vector<std::string>* neighbors)
{
	int x = tile->position().first;
	int y = tile->position().second;

	//left
	if (x - 1 >= 0)
		neighbors->push_back(createStringWithFormat("%d, %d", (int)(x - 1), (int)y));
	//right
	if (x + 1 < m_mapWidth)
		neighbors->push_back(createStringWithFormat("%d, %d", (int)(x + 1), (int)y));
	//top
	if (y - 1 >= 0)
		neighbors->push_back(createStringWithFormat("%d, %d", (int)x, (int)(y - 1)));
	//bottom
	if (y + 1 < m_mapHeight)
		neighbors->push_back(createStringWithFormat("%d, %d", (int)x, (int)(y + 1)));
	if (m_diagonalRule)
	{
		//left-top
		if (x - 1 >= 0 && y - 1 >= 0);
		neighbors->push_back(createStringWithFormat("%d, %d", (int)(x - 1), (int)(y - 1)));
		//left-bottom
		if (x - 1 >= 0 && y + 1 < m_mapHeight)
			neighbors->push_back(createStringWithFormat("%d, %d", (int)(x - 1), (int)(y + 1)));
		//right-top
		if (x + 1 < m_mapWidth && y - 1 >= 0)
			neighbors->push_back(createStringWithFormat("%d, %d", (int)(x + 1), (int)(y - 1)));
		//right-bottom
		if (x + 1 < m_mapWidth && y + 1 < m_mapHeight)
			neighbors->push_back(createStringWithFormat("%d, %d", (int)(x + 1), (int)(y + 1)));
	}
}


int RouteAlgorithm::computeHScore(const TileData* tile, const TileData* endTile)
{
	if (tile == nullptr || endTile == nullptr)
		return 0xffff;

	std::pair<float, float> originPosition = tile->position();
	std::pair<float, float> endTilePosition = endTile->position();

	int absx = std::abs((int)endTilePosition.first - (int)originPosition.first);
	int absy = std::abs((int)endTilePosition.second - (int)originPosition.second);

	return (absx + absy) * 10;
}


void RouteAlgorithm::insertToOpenList(TileData* tile)
{
	int fscore = tile->fscore();

	int index = 0;
	for each (TileData* tmp in m_openList)
	{
		if (tmp->fscore() >= fscore)
			break;

		++index;
	}

	m_openList.insert(m_openList.begin() + index, tile);
}

std::pair<float, float> RouteAlgorithm::String2Pair(const std::string* _str)
{
	const char* split = ",";
	std::vector<std::string> strs;

	std::string tmpstr(_str->c_str());
	char* pstr = strtok((char*)tmpstr.c_str(), split);
	while (pstr != NULL)
	{
		strs.push_back(pstr);
		pstr = strtok(NULL, split);
	}

	if (strs.size() < 2)
		return std::make_pair(0, 0);

	float first = atoi(strs[0].c_str());
	float second = atoi(strs[1].c_str());

	return std::make_pair(first, second);
}

int RouteAlgorithm::moveCost(const TileData* originTile, const TileData* destinationTile)
{
	std::pair<float, float> origin = originTile->position();
	std::pair<float, float> destination = destinationTile->position();

	float sum = std::pow(destination.first - origin.first, 2.0f)
		+ std::pow(destination.second - origin.second, 2.0f);

	return (int)(std::sqrtf(sum) * 10) - destinationTile->priority();
}

void RouteAlgorithm::constructPath(const TileData* endTile)
{
	if (endTile == nullptr) return;

	if(endTile->walkAble())
		m_route.push_back((TileData*)endTile);

	TileData* parent = endTile->parent();
	while (parent != nullptr)
	{
		TileData* tmp = parent;
		parent = parent->parent();
		m_route.push_back(tmp);
	}
}

std::string RouteAlgorithm::createStringWithFormat(const char* format, ...)
{
	char buffer[1024] = { 0 };
	va_list arg_ptr;

	va_start(arg_ptr, format);
	vsprintf(buffer, format, arg_ptr);
	va_end(arg_ptr);

	return buffer;
}

std::vector<TileData*> RouteAlgorithm::path() const
{
	return m_route;
}

std::set<TileData*> RouteAlgorithm::nearestNeighborWalkAbleTile(TileData* tile)
{
	std::set<TileData*> neighbors;

	if(tile->walkAble())
	{
		neighbors.insert(neighbors.begin(), tile);
		return neighbors;
	}

	m_neighborCloseList.push_back(tile);

	std::vector<std::string> temp;
	std::vector<TileData*> tempTiles;
	Neighbor(tile, &temp);
	for each (std::string str in temp)
	{
		std::pair<float, float> pair = String2Pair(&str);
		TileData* tile  = getTileDataByPosition(pair);
		std::vector<TileData*>::iterator iter = std::find(m_neighborCloseList.begin()
			, m_neighborCloseList.end(), tile);
		if(iter == m_neighborCloseList.end())
			tempTiles.push_back(tile);
	}

	for each (TileData* tile in tempTiles)
	{
		if (tile->walkAble())
			neighbors.insert(neighbors.begin(), tile);
	}

	if (neighbors.size() == 0)
	{
		for each (TileData* tile in tempTiles)
		{
			std::set<TileData*> tmpvec = nearestNeighborWalkAbleTile(tile);
			neighbors.insert(tmpvec.begin(), tmpvec.end());
			m_neighborCloseList.push_back(tile);
			if (neighbors.size() > 0)
				break;
		}
	}

	return neighbors;
}

void RouteAlgorithm::GetRoute(TileData* startTile, TileData* endTile)
{
	m_route.clear();
	m_openList.clear();
	m_closeList.clear();

	startTile->setParent(nullptr);
	m_openList.push_back(startTile);
	do
	{
		TileData* tile = (TileData*)m_openList[0];
		m_openList.erase(m_openList.begin());
		m_closeList.push_back(tile);
		if (tile == endTile)
		{
			constructPath(tile);
			break;
		}

		std::vector<std::string> neighbors;
		Neighbor(tile, &neighbors);
		for each (std::string str in neighbors)
		{
			std::string* pstr = &str;
			TileData* neighborTileData = getTileDataByPosition(String2Pair(pstr));
			std::vector<TileData*>::iterator closeListIter = std::find(m_closeList.begin(), m_closeList.end()
				, neighborTileData);
			if (!neighborTileData || (!neighborTileData->walkAble() && neighborTileData != endTile)
				|| closeListIter != m_closeList.end())
				continue;

			int cost = moveCost(tile, neighborTileData);
			int hscore = computeHScore(neighborTileData, endTile);
			std::vector<TileData*>::iterator openListIter = std::find(m_openList.begin(), m_openList.end()
				, neighborTileData);
			if (openListIter == m_openList.end())
			{
				neighborTileData->setGscore(tile->gscore() + cost);
				neighborTileData->setHscore(hscore);
				neighborTileData->setParent(tile);
				insertToOpenList(neighborTileData);
			}
			else
			{
				if (tile->gscore() + cost < neighborTileData->gscore())
				{
					std::remove(m_openList.begin(), m_openList.end(), neighborTileData);
					neighborTileData->setParent(tile);
					neighborTileData->setGscore(tile->gscore() + cost);
					insertToOpenList(neighborTileData);
				}
			}

		}

	} while (m_openList.size() > 0);

}