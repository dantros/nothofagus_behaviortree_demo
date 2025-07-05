#pragma once

#include <initializer_list>
#include <nothofagus.h>

#define RUNTIME_CHECK(value) if (not (value)) throw;

struct Gamepad
{
    bool up = false;
    bool left = false;
    bool down = false;
    bool right = false;
};

class GameObject
{
public:
    void init(Nothofagus::Canvas* canvasPtr, Nothofagus::BellotaId bellotaId);
    Nothofagus::Bellota& getBellota();
    virtual void onCreate() {};
    virtual void update(float deltaTime) {};
    virtual void onDestroy() {};

protected:
    Nothofagus::Canvas* mCanvasPtr;
    Nothofagus::BellotaId mBellotaId;
};

void registerContinuosAction(Nothofagus::Controller& controller, Nothofagus::Key key, bool& button);

void registerContinuosAction(Nothofagus::Controller& controller, std::initializer_list<Nothofagus::Key> keys, bool& button);