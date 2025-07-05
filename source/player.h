#pragma once

#include <nothofagus.h>
#include "basic_game_engine.h"

class Player : public GameObject
{
public:
    void init(Nothofagus::Canvas* canvasPtr, Nothofagus::BellotaId bellotaId, float speed, Gamepad* gamepadPtr);

    virtual void onCreate() override;
    virtual void update(float deltaTime) override;
    virtual void onDestroy() override;

protected:
    float mSpeed;
    Gamepad* mGamepadPtr;
};