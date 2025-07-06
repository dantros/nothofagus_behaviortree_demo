#pragma once

#include "basic_game_engine.h"
#include "player.h"
#include "velocity_component.h"
#include <string>

// pImpl idiom to hide implementation dependencies into the translation unit
struct BehaviorTree
{
    struct Impl;
    Impl* pImpl;
    std::string mDebugMessage;
};

class SmartFollower : public GameObject
{
public:
    SmartFollower();

    // allocate pIml idiom
    void init(Nothofagus::Canvas* canvasPtr, Nothofagus::BellotaId bellotaId, float speed, Player* playerPtr, float danceDuration, float danceSpeed);
    
    // deallocate pImpl idiom
    ~SmartFollower();

    virtual void onCreate() override;
    virtual void update(float deltaTime) override;
    virtual void onDestroy() override;

protected:
    Player* mPlayerPtr;
    
public:
    BehaviorTree mBehaviorTree;
    VelocityComponent mVelocityComponent;
    float getDeltaTime() const;
    const std::string& getDebugMessage() const;
};

