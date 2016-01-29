#include "RouteAlgorithm.h"

RouteAlgorithm::RouteAlgorithm(bool allowDiagonal) : m_allowDiagonal(allowDiagonal)
{
	m_openList.clear();
	m_closeList.clear();
}

RouteAlgorithm::RouteAlgorithm(int width, int height, const std::vector<TileData*> mapTileDatas
	, bool allowDiagonal) : m_mapWidth(width), m_mapHeight(height), m_tileDatas(mapTileDatas)
						, m_allowDiagonal(allowDiagonal)
{
	m_openList.clear();
	m_closeList.clear();
}

RouteAlgorithm::~RouteAlgorithm()
{

}

void RouteAlgorithm::setAllowDiagonal(bool allowDiagonal)
{
	m_allowDiagonal = allowDiagonal;
}

bool RouteAlgorithm::allowDiagonal() const
{
	return m_allowDiagonal;
}

void RouteAlgorithm::setAllowCrossConrner(bool allowCrossConrner)
{
	m_allowCrossCorner = allowCrossConrner;
}

bool RouteAlgorithm::allowCrossConrner() const
{
	return m_allowCrossCorner;
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
	m_route.clear();
	m_openList.clear();
	m_closeList.clear();

	startTile->setParent(NULL);
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

		std::vector<TileData*> neighbors;
		Neighbor(tile, &neighbors);
		for each (TileData* neighborTileData in neighbors)
		{
			std::vector<TileData*>::iterator closeListIter = std::find(m_closeList.begin(), m_closeList.end()
				, neighborTileData);
			if (!neighborTileData || closeListIter != m_closeList.end())
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

void RouteAlgorithm::Neighbor(const TileData* tile, std::vector<TileData*>* neighbors)
{
	int x = tile->position().first;
	int y = tile->position().second;

	TileData* left = NULL;
	TileData* right = NULL;
	TileData* bottom = NULL;
	TileData* top = NULL;
	//left
	if (x - 1 >= 0)
	{
		left = getTileDataByPosition(std::make_pair(x - 1, y));
		neighbors->push_back(left);
	}
	//right
	if (x + 1 < m_mapWidth)
	{
		right = getTileDataByPosition(std::make_pair(x + 1, y));
		neighbors->push_back(right);
	}
	//top
	if (y - 1 >= 0)
	{
		top = getTileDataByPosition(std::make_pair(x, y - 1));
		neighbors->push_back(top);
	}

	//bottom
	if (y + 1 < m_mapHeight)
	{
		bottom = getTileDataByPosition(std::make_pair(x, y + 1));
		neighbors->push_back(bottom);
	}

	if (m_allowDiagonal)
	{
		//left-top
		if (x - 1 >= 0 && y - 1 >= 0);
		{
			if ((left == NULL || left->walkAble()) && (top == NULL || top->walkAble()))
				neighbors->push_back(getTileDataByPosition(std::make_pair(x - 1, y - 1)));
		}
		//left-bottom
		if (x - 1 >= 0 && y + 1 < m_mapHeight)
			if ((left == NULL || left->walkAble()) && (bottom == NULL || bottom->walkAble()))
				neighbors->push_back(getTileDataByPosition(std::make_pair(x - 1, y + 1)));
		//right-top
		if (x + 1 < m_mapWidth && y - 1 >= 0)
			if((right == NULL || right->walkAble()) && (top == NULL || top->walkAble()))
				neighbors->push_back(getTileDataByPosition(std::make_pair(x + 1, y - 1)));
		//right-bottom
		if (x + 1 < m_mapWidth && y + 1 < m_mapHeight)
			if ((right == NULL || right->walkAble()) && (bottom == NULL || bottom->walkAble()))
				neighbors->push_back(getTileDataByPosition(std::make_pair(x + 1, y + 1)));
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

	return (int)(std::sqrtf(sum) * 10) - destinationTile->priority() + destinationTile->extraHScore();
}

void RouteAlgorithm::constructPath(const TileData* endTile)
{
	if (endTile == nullptr) return;

	std::vector<TileData*> tmp_route;
	tmp_route.push_back((TileData*)endTile);

	TileData* parent = endTile->parent();
	while (parent != NULL)
	{
		TileData* tmp = parent;
		parent = parent->parent();
		tmp_route.push_back(tmp);
	}

	std::vector<TileData*>::iterator iter = tmp_route.begin();
	while (iter != tmp_route.end())
	{
		if (!(*iter)->walkAble())
		{
			++iter;
			break;
		}
		++iter;
	}

	if (iter != tmp_route.end())
		m_route.insert(m_route.begin(), iter, tmp_route.end());
	else
		m_route = tmp_route;

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

/*
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
*/