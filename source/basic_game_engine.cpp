
#include "basic_game_engine.h"

void GameObject::init(Nothofagus::Canvas* canvasPtr, Nothofagus::BellotaId bellotaId)
{
    mCanvasPtr = canvasPtr;
    mBellotaId = bellotaId;
}

Nothofagus::Bellota& GameObject::getBellota()
{
    RUNTIME_CHECK(mCanvasPtr);
    return mCanvasPtr->bellota(mBellotaId);
}

void registerContinuosAction(Nothofagus::Controller& controller, Nothofagus::Key key, bool& button)
{
    controller.registerAction({key, Nothofagus::DiscreteTrigger::Press}, [&]()
    {
        button = true;
    });
    controller.registerAction({key, Nothofagus::DiscreteTrigger::Release}, [&]()
    {
        button = false;
    });
}

void registerContinuosAction(Nothofagus::Controller& controller, std::initializer_list<Nothofagus::Key> keys, bool& button)
{
    for (auto& key : keys)
        registerContinuosAction(controller, key, button);
}