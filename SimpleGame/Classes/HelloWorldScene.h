#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

#define ballCount 20

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    void initBalls();
    
    void initSquares();
    
    void initEdge();
    
    void initTouchListener();
    
    void initCollisionListener();
    
    void initCloseMenu();
    
    bool onContactBegin(cocos2d::PhysicsContact& contact);
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
private:
    cocos2d::Sprite* sprite;
    cocos2d::Point startPoint;
    cocos2d::PhysicsBody* physicsBodyBall;
    std::vector<cocos2d::PhysicsBody*> physicsBodyBallVector;
    std::vector<cocos2d::Sprite*> spriteVector;
    std::vector<cocos2d::Follow*> followVector;
};

#endif // __HELLOWORLD_SCENE_H__
