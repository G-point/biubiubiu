#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include <iostream>
#include <unistd.h>

using namespace std;

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    if ( !Scene::initWithPhysics() )
    {
        return false;
    }
    initCloseMenu();
    initBalls();
    initListeners();
    initSquares();
    initEdge();
    
    return true;
}

void HelloWorld::initBalls()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    for (int i=0; i<ballCount; i++)
    {
        physicsBodyBallVector.push_back(PhysicsBody::createCircle(10.0f, PhysicsMaterial(0.1f, 1.0f, 0.0f)));
        spriteVector.push_back(Sprite::create("ball1.png"));
    if (spriteVector[i] != nullptr)
    {
        physicsBodyBallVector[i]->setGravityEnable(false);
        physicsBodyBallVector[i]->setCategoryBitmask(0x01);
        physicsBodyBallVector[i]->setCollisionBitmask(0x02);
        // position the sprite on the center of the screen
        spriteVector[i]->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
        spriteVector[i]->addComponent(physicsBodyBallVector[i]);
        // add the sprite as a child to this layer
        this->addChild(spriteVector[i], 0);
        if (i>0)
        {
            usleep(500*i);
            //followVector.push_back(Follow::create(spriteVector[i-1]));
            //followVector[i]->initWithTarget(spriteVector[i-1]);
            spriteVector[i]->runAction(Follow::create(spriteVector[i-1]));
        }
    }
    else
    {
        problemLoading("'ball1.png'");
    }
    }
}

void HelloWorld::initSquares()
{
    auto spriteSquare = Sprite::create("square.png");
    if (spriteSquare != nullptr)
    {
        // position the sprite on the center of the screen
        spriteSquare->setPosition(Vec2(300, 100));
        auto physicsBody = PhysicsBody::createBox(Size(35.0f, 35.0f),
                                                  PhysicsMaterial(0.1f, 1.0f, 0.0f));
        physicsBody->setDynamic(false);
        spriteSquare->addComponent(physicsBody);
        // add the sprite as a child to this layer
        this->addChild(spriteSquare, 0);
    }
    else
    {
        problemLoading("'square.png'");
    }
}

void HelloWorld::initEdge()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    auto edgeSp = Sprite::create();
    auto boundBody = PhysicsBody::createEdgeBox(visibleSize,PhysicsMaterial(0.0f,1.0f,0.0f),3);
    edgeSp->setPosition(origin.x + visibleSize.width/2, origin.y + visibleSize.height/2);
    edgeSp->setPhysicsBody(boundBody);
    addChild(edgeSp);
}

void HelloWorld::initListeners()
{
    // 创建一个事件监听器类型为 OneByOne 的单点触摸
    auto listener1 = EventListenerTouchOneByOne::create();
    // 设置是否吞没事件，在 onTouchBegan 方法返回 true 时吞没
    listener1->setSwallowTouches(true);
    
    // 使用 lambda 实现 onTouchBegan 事件回调函数
    listener1->onTouchBegan = [this](Touch* touch, Event* event){
        // 获取事件所绑定的 target
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        
        Point locationInNode = touch->getLocation();
        startPoint = locationInNode;
        log("sprite began... x = %f, y = %f", locationInNode.x, locationInNode.y);
        target->setPosition(Vec2(locationInNode.x, locationInNode.y));
        target->setOpacity(180);
        physicsBodyBallVector[0]->setVelocity(Vec2(0, 0));
        return true;
    };
    
    listener1->onTouchMoved = [this](Touch* touch, Event* event){
        // 获取事件所绑定的 target
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        
        // 获取当前点击点所在相对按钮的位置坐标
        Point locationInNode = touch->getLocation();
        auto y_offset = locationInNode.y - startPoint.y;
        auto x_offset = locationInNode.x - startPoint.x;
        auto angle = abs(x_offset) > 0.0001 ? atan(abs(y_offset/x_offset)) * 180 / 3.14159265359 : 0;
        log("sprite move... angle = %f", angle);
        //sprite->setScale(1, 1+y_offset/20.0f);
        //sprite->setRotation(abs(x_offset) > 0.0001 ? atan(y_offset/x_offset) : 0);
        target->setOpacity(180);
        return true;
    };
    
    listener1->onTouchEnded = [this](Touch* touch, Event* event){
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        Point locationInNode = touch->getLocation();
        auto y_offset = locationInNode.y - startPoint.y;
        auto x_offset = locationInNode.x - startPoint.x;
        target->setOpacity(180);
        auto angle = abs(x_offset) > 0.0001 ? atan(y_offset/x_offset) : 0;
        physicsBodyBallVector[0]->setVelocity(Vec2(300*cos(angle), 300*sin(angle)));
        return true;
    };
    
    // 添加监听器
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, spriteVector[0]);
}

void HelloWorld::initCloseMenu()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }
    
    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
    #endif

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}

