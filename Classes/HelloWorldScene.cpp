#include "HelloWorldScene.h"

USING_NS_CC;

const static std::string DrawNodeStr = "drawnode";

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

	m_gamemap = NULL;
	m_player = NULL;
	m_routeAlgorithm = NULL;

	loadMap("gamemap.tmx");
 	loadGamePlayer();
 	initTileDatas();

	//初始化寻路算法的地图数据
	Size mapsize = m_gamemap->getMapSize();
	m_routeAlgorithm = new RouteAlgorithm(mapsize.width, mapsize.height, m_tileDatas);
	m_routeAlgorithm->setAllowDiagonal(true);//允许对角线
	m_routeAlgorithm->setAllowCrossConrner(false);//不允许穿越拐角
	assert(m_routeAlgorithm != NULL);

	scheduleUpdate();
	setTouchEnabled(true);

    return true;
}

void HelloWorld::onExit()
{
	delete m_routeAlgorithm;
	m_routeAlgorithm = NULL;

    for (TileData* tile : m_tileDatas)
	{
		delete tile;
		tile = NULL;
	}
	m_tileDatas.clear();

	Layer::onExit();
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

/**
 *加载地图
*/
void HelloWorld::loadMap(const std::string& mapName)
{
	String* mapXml = String::createWithContentsOfFile("gamemap.tmx");
	m_gamemap = TMXTiledMap::createWithXML(mapXml->getCString(), "");
	assert(m_gamemap);
	TMXLayer* layer = m_gamemap->layerNamed("block");
	if (layer != NULL)
		layer->setVisible(false);

	this->addChild(m_gamemap);
}

/*
 *加载游戏玩家
*/
void HelloWorld::loadGamePlayer()
{
	TMXObjectGroup* objGroup = m_gamemap->getObjectGroup("objectlayer");
	ValueMap showPoint = objGroup->getObject("PlayerShowPoint");
	int x = showPoint["x"].asInt();
	int y = showPoint["y"].asInt();

	m_player = Sprite::create("029.png");
	m_player->setPosition(Vec2(x + m_player->getContentSize().width / 2
		, y + m_player->getContentSize().height / 2));
	m_player->setPosition(m_player->getPosition());
	addChild(m_player);
}

void HelloWorld::onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *unused_event)
{
	m_touchMoved = false;
}


void HelloWorld::onTouchesMoved(const std::vector<Touch*>& touches, Event *unused_event)
{
    for (Touch* touch : touches)
	{
		Vec2 delta = touch->getDelta();

		Vec2 min(-m_gamemap->getContentSize().width + getContentSize().width
			, -m_gamemap->getContentSize().height + getContentSize().height);
		Vec2 max(0, 0);
		Vec2 dst;
		Vec2::clamp(getPosition() + delta, min, max, &dst);
		setPosition(dst);//改变层的位置
	}
	m_touchMoved = true;
}

void HelloWorld::onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *unused_event)
{
	if (m_touchMoved) return;

	m_player->stopAllActions();

    for (Touch* touch : touches)
	{
		Vec2 pos = touch->getLocation();
		TileData* startTile = tileDataByTileCoordinate(convertToTileCoordinate(m_player->getPosition()));
		TileData* endTile = tileDataByTileCoordinate(convertToTileCoordinate(convertToNodeSpace(touch->getLocation())));
		m_routeAlgorithm->findingPath(startTile, endTile);
		std::vector<TileData*> path = m_routeAlgorithm->path();

		if (path.size() > 0)//当前位置可以达到
		{
			//输出路径
			//drawPath(path);
			createActionChain(path);
		}
	}
}

void HelloWorld::update(float delta)
{
	Layer::update(delta);
	setViewPointCenter(m_player->getPosition());
}


void HelloWorld::setViewPointCenter(const cocos2d::Vec2& position)
{
	Size winsize = Director::sharedDirector()->getWinSize();

	//相对于窗口左下角的坐标
	Vec2 relativeToWindow = position + this->getPosition();

	Vec2 pos = getPosition() - relativeToWindow + Vec2(winsize.width/2, winsize.height/2);

	Vec2 max(0, 0);
	Vec2 min(winsize.width - m_gamemap->getContentSize().width
		, winsize.height - m_gamemap->getContentSize().height);

	Vec2 actualPos;
	Vec2::clamp(pos, min, max, &actualPos);

	this->setPosition(actualPos);//改变层的位置
}

Vec2 HelloWorld::convertToTileCoordinate(const cocos2d::Vec2& vec)
{
	Size tileSize = m_gamemap->getTileSize();
	Size mapSize = m_gamemap->getMapSize();

	Vec2 relativeLeftTop;
	relativeLeftTop.x = vec.x;
	relativeLeftTop.y = abs(vec.y - mapSize.height * tileSize.height);

	return Vec2((int)(relativeLeftTop.x / tileSize.width), (int)(relativeLeftTop.y / tileSize.height));
}

TileData* HelloWorld::tileDataByTileCoordinate(const cocos2d::Vec2& vec)
{
    for (TileData* tile : m_tileDatas)
	{
		if (tile)
		{
			std::pair<float, float> pos = tile->position();
			if ((int)pos.first == vec.x && (int)pos.second == vec.y)
				return tile;
		}
	}

	return NULL;
}

void HelloWorld::initTileDatas()
{
	Size mapsize = m_gamemap->getMapSize();
	TMXLayer* layer = m_gamemap->layerNamed("block");
	assert(layer != NULL);

	for (int m = 0; m < mapsize.width; ++m)
	{
		for (int n = 0; n < mapsize.height; ++n)
		{
			uint32_t gid = layer->getTileGIDAt(Vec2(m, n));
			TileData* data = new TileData(std::make_pair(m, n));
			if (gid)
			{
				ValueMap property = m_gamemap->getPropertiesForGID(gid).asValueMap();

				if (property["block"].asString() == "true")
					data->setWalkAble(false);
				data->setPriority(property["Priority"].asInt());
				data->setExtraHScore(property["extra"].asInt());
			}
			m_tileDatas.push_back(data);
		}
	}
}

void HelloWorld::createActionChain(const std::vector<TileData*>& path)
{
	if (path.size() <= 0) return;

	Size mapSize = m_gamemap->getMapSize();
	Size tilesize = m_gamemap->getTileSize();
	int mapheight = mapSize.height * tilesize.height;

	Vector<FiniteTimeAction*> actions;

	std::vector<TileData*>::const_reverse_iterator rIter = path.rbegin();
	rIter++;
	while (rIter != path.rend())
	{
		int destination_x = (*rIter)->position().first * tilesize.width + tilesize.width / 2;
		int destination_y = (*rIter)->position().second * tilesize.height + tilesize.height / 2;
		Vec2 destination(destination_x, mapheight - destination_y);
		MoveTo* action = MoveTo::create(0.5f, destination);
		actions.pushBack(action);

		rIter++;
	}

	if (actions.size() > 0)
	{
		Sequence* action_seq = Sequence::create(actions);
		m_player->runAction(action_seq);
	}
}



#ifdef _DEBUG
void HelloWorld::drawPath(const std::vector<TileData*>& path)
{
	this->removeChildByName(DrawNodeStr);

	DrawNode* drawNode = DrawNode::create();

	Size mapSize = m_gamemap->getMapSize();
	Size tilesize = m_gamemap->getTileSize();
	int mapheight = mapSize.height * tilesize.height;

	int origin_x = path[0]->position().first * tilesize.width + tilesize.width / 2;
	int origin_y = path[0]->position().second * tilesize.height + tilesize.height / 2;
	Vec2 origin(origin_x, mapheight - origin_y);
	for (int index = 1; index < path.size(); ++index)
	{
		int destination_x = path[index]->position().first * tilesize.width + tilesize.width / 2;
		int destination_y = path[index]->position().second * tilesize.height + tilesize.height / 2;
		Vec2 destination(destination_x, mapheight - destination_y);
		drawNode->drawLine(origin, destination, Color4F(0.0, 1.0, 0.0, 1.0));
		origin = destination;
	}

	this->addChild(drawNode, 0, DrawNodeStr);
}
#endif