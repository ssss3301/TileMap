#pragma once

#include <utility>
#include "cocos2d.h"

class TileData
{
public:
	TileData();
	TileData(const std::pair<float, float>& position);
	~TileData();

	void setWalkAble(bool walkAble);
	bool walkAble() const;

	void setPriority(int priority);
	int priority() const;

	void setParent(TileData* parent);
	TileData* parent() const;

	void setPosition(const std::pair<float, float>& position);
	std::pair<float, float> position() const;

	void setIsEnd(bool isEnd);
	bool isEnd() const;

	void setGscore(int score);
	int gscore() const;

	void setHscore(int score);
	int hscore() const;

	int fscore() const;

private:
	bool						m_bWalkable;
	int							m_priority;
	TileData*					m_parent;
	std::pair<float, float>		m_position;
	bool						m_isEnd;
	int							m_GScore;
	int							m_HScore;
};