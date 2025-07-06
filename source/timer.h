#pragma once

class Timer
{
public:
    Timer(float maxTime) : mCurrentTime(0.f), mMaxTime(maxTime)
    {}

    void reset()
    {
        mCurrentTime = 0.f;
    }

    void update(float deltaTime)
    {
        mCurrentTime += deltaTime;
    }

    float normalizedTime() const
    {
        return mCurrentTime / mMaxTime;
    }

    bool isFinished() const
    {
        return mCurrentTime >= mMaxTime;
    }

private:
    float mCurrentTime;
    float mMaxTime;
};