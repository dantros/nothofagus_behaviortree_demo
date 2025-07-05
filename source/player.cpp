
#include "player.h"

void Player::init(Nothofagus::Canvas* canvasPtr, Nothofagus::BellotaId bellotaId, float speed, Gamepad* gamepadPtr)
{
    GameObject::init(canvasPtr, bellotaId);
    mSpeed = speed;
    mGamepadPtr = gamepadPtr;
}

void Player::onCreate()
{
}

void Player::update(float deltaTime)
{
    Nothofagus::Bellota& bellota = getBellota();

    RUNTIME_CHECK(mGamepadPtr);
    if (mGamepadPtr->left)
        bellota.transform().location().x -= mSpeed * deltaTime;

    if (mGamepadPtr->right)
        bellota.transform().location().x += mSpeed * deltaTime;

    if (mGamepadPtr->up)
        bellota.transform().location().y += mSpeed * deltaTime;

    if (mGamepadPtr->down)
        bellota.transform().location().y -= mSpeed * deltaTime;

    RUNTIME_CHECK(mCanvasPtr);
    const Nothofagus::ScreenSize& screenSize = mCanvasPtr->screenSize();
    bellota.transform().location().x = std::clamp<float>(bellota.transform().location().x, 10, screenSize.width - 10);
    bellota.transform().location().y = std::clamp<float>(bellota.transform().location().y, 10, screenSize.height - 10);
}

void Player::onDestroy()
{
}
