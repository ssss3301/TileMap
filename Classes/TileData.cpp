#include "TileData.h"

USING_NS_CC;

TileData::TileData():m_bWalkable(true), m_parent(NULL), m_isEnd(false)
		, m_GScore(0), m_HScore(0), m_priority(0), m_extraHScore(0)
{

}

TileData::TileData(const std::pair<float, float>& position) : m_bWalkable(true), m_parent(NULL)
	, m_isEnd(false), m_GScore(0), m_HScore(0), m_priority(0), m_extraHScore(0)
{
	m_position = position;
}

TileData::~TileData()
{

}


void TileData::setWalkAble(bool walkAble)
{
	m_bWalkable = walkAble;
}

bool TileData::walkAble() const
{
	return m_bWalkable;
}

void TileData::setPriority(int priority)
{
	m_priority = priority;
}

int TileData::priority() const
{
	return m_priority;
}


void TileData::setParent(TileData* parent)
{
	m_parent = parent;
}

TileData* TileData::parent() const
{
	return m_parent;
}


void TileData::setPosition(const std::pair<float, float>& position)
{
	m_position = position;
}

std::pair<float, float> TileData::position() const
{
	return m_position;
}

void TileData::setIsEnd(bool isEnd)
{
	m_isEnd = isEnd;
}

bool TileData::isEnd() const
{
	return m_isEnd;
}

void TileData::setGscore(int score)
{
	m_GScore = score;
}

int TileData::gscore() const
{
	return m_GScore;
}

void TileData::setHscore(int score)
{
	m_HScore = score;
}

int TileData::hscore() const
{
	return m_HScore;
}

void TileData::setExtraHScore(int extraScore)
{
	m_extraHScore = extraScore;
}

int TileData::extraHScore() const
{
	return m_extraHScore;
}


int TileData::fscore() const
{
	return m_HScore + m_GScore;
}
