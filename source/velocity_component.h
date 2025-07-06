#pragma once

#include "basic_game_engine.h"
#include <glm/glm.hpp>

class VelocityComponent
{
public:
    void init(const glm::vec2& direction, float speed)
    {
        mDirection = glm::normalize(direction);
        mSpeed = speed;
    }

    float getSpeed() const
    {
        return mSpeed;
    }

    void setSpeed(float speed)
    {
        mSpeed = speed;
    }

    glm::vec2 getDirection() const
    {
        return mDirection;
    }

    // Direction is normalized internally
    void setDirection(const glm::vec2& direction)
    {
        mDirection = glm::normalize(direction);
    }

    void move(GameObject* ownerPtr, float deltaTime) const
    {
        RUNTIME_CHECK(ownerPtr);        
        ownerPtr->getBellota().transform().location() += (deltaTime * mSpeed * mDirection);
    }

private:
    glm::vec2 mDirection;
    float mSpeed;
};