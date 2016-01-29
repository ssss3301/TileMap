#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "RouteAlgorithm.h"

#include <vector>

class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

	virtual void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *unused_event);
	virtual void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *unused_event);
	virtual void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *unused_event);
	virtual void onExit();
	virtual void update(float delta);

private:
	void loadMap(const std::string& mapName);
	void loadGamePlayer();
	void setViewPointCenter(const cocos2d::Vec2& position);
	cocos2d::Vec2 convertToTileCoordinate(const cocos2d::Vec2& vec);
	TileData* tileDataByTileCoordinate(const cocos2d::Vec2& vec);
	void initTileDatas();
	void createActionChain(const std::vector<TileData*>& path);
		
#ifdef _DEBUG
	void drawPath(const std::vector<TileData*>& path);
#endif

private:
	cocos2d::TMXTiledMap*	m_gamemap;
	cocos2d::Sprite* m_player;
	RouteAlgorithm*	m_routeAlgorithm;
	std::vector<TileData*>	m_tileDatas;

	bool	m_touchMoved;
};

#endif // __HELLOWORLD_SCENE_H__
