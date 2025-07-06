
#include "smart_follower.h"
#include <behaviortree_cpp/behavior_tree.h>
#include <behaviortree_cpp/bt_factory.h>
#include <format>
#include "timer.h"

// Helper function to deserialize a location into a glm::vec2 object.
namespace BT
{
    template <> inline glm::vec2 convertFromString(StringView str)
    {
        auto parts = splitString(str, ';');
        if (parts.size() != 2)
        {
            throw RuntimeError("invalid input)");
        }
        else
        {
            glm::vec2 output;
            output.x = convertFromString<double>(parts[0]);
            output.y = convertFromString<double>(parts[1]);
            return output;
        }
    }
} // namespace BT

namespace ActionNodes
{

class LookForPlayer : public BT::SyncActionNode
{
public:
    LookForPlayer(const std::string& name, const BT::NodeConfig& config, SmartFollower* followerPtr, Player* playerPtr) :
        BT::SyncActionNode(name, config),
        mSmartFollowerPtr(followerPtr),
        mPlayerPtr(playerPtr)
    {
    }

    static BT::PortsList providedPorts()
    {
        return {
            BT::OutputPort<glm::vec2>("player_location")
        };
    }

    virtual BT::NodeStatus tick() override
    {
        // if by any change the player becomes nullptr, this task will fail
        // you may want to consider a fallback task to gather the player location or to perform a specific task when there is no player.
        if (mPlayerPtr)
        {
            glm::vec2 playerLocation = mPlayerPtr->getBellota().transform().location();
            setOutput("player_location", std::format("{};{}", playerLocation.x, playerLocation.y) );
            return BT::NodeStatus::SUCCESS;
        }
        else
        {
            return BT::NodeStatus::FAILURE;
        }
    }

private:
    SmartFollower* mSmartFollowerPtr;
    Player* mPlayerPtr;
};

class CloseEnough : public BT::SyncActionNode
{
public:
    CloseEnough(const std::string& name, const BT::NodeConfig& config, SmartFollower* followerPtr, float threshold) :
        BT::SyncActionNode(name, config),
        mSmartFollowerPtr(followerPtr),
        mThreshold(threshold)
    {
    }

    static BT::PortsList providedPorts()
    {
        return {
            BT::InputPort<glm::vec2>("player_location")
        };
    }

    virtual BT::NodeStatus tick() override
    {
        BT::Expected<glm::vec2> playerLocationInput = getInput<glm::vec2>("player_location");
        if (!playerLocationInput)
            throw BT::RuntimeError("missing required input [player_location]: ", playerLocationInput.error() );

        glm::vec2 playerLocation = playerLocationInput.value();

        // if by any chance the player becomes nullptr, we will throw an exception, we never want to handle this case.
        RUNTIME_CHECK(mSmartFollowerPtr);
        glm::vec2 botLocation = mSmartFollowerPtr->getBellota().transform().location();

        if (glm::distance(playerLocation, botLocation) < mThreshold)
            return BT::NodeStatus::SUCCESS;
        else
            return BT::NodeStatus::FAILURE;
    }

private:
    SmartFollower* mSmartFollowerPtr;
    float mThreshold;
};

class MoveTo : public BT::StatefulActionNode
{
public:
    MoveTo(const std::string& name, const BT::NodeConfig& config, SmartFollower* followerPtr, float threshold) :
        BT::StatefulActionNode(name, config),
        mSmartFollowerPtr(followerPtr),
        mThreshold(threshold)
    {
    }

    static BT::PortsList providedPorts()
    {
        return {
            BT::InputPort<glm::vec2>("destination")
        };
    }

    virtual BT::NodeStatus onStart() override
    {
        // getting destination
        BT::Expected<glm::vec2> destinationInput = getInput<glm::vec2>("destination");
        if (!destinationInput)
            throw BT::RuntimeError("missing required input [destination]: ", destinationInput.error() );
        mDestination = destinationInput.value();

        // computing and setting direction
        RUNTIME_CHECK(mSmartFollowerPtr);
        Nothofagus::Bellota& bellota = mSmartFollowerPtr->getBellota();
        const glm::vec2& currentLocation = bellota.transform().location();
        const glm::vec2 direction = mDestination - currentLocation;
        mSmartFollowerPtr->mVelocityComponent.setDirection(direction);

        return BT::NodeStatus::RUNNING;
    }

    virtual BT::NodeStatus onRunning() override
    {
        RUNTIME_CHECK(mSmartFollowerPtr);
        Nothofagus::Bellota& bellota = mSmartFollowerPtr->getBellota();
        const glm::vec2& currentLocation = bellota.transform().location();

        if (glm::distance(currentLocation, mDestination) < mThreshold)
        {
            return BT::NodeStatus::SUCCESS;
        }

        const float deltaTime = mSmartFollowerPtr->getDeltaTime();
        mSmartFollowerPtr->mVelocityComponent.move(mSmartFollowerPtr, deltaTime);
        return BT::NodeStatus::RUNNING;
    }

    virtual void onHalted() override
    {
    }

private:
    SmartFollower* mSmartFollowerPtr;
    glm::vec2 mDestination;
    float mThreshold;
};

class Dance : public BT::StatefulActionNode
{
public:
    Dance(const std::string& name, const BT::NodeConfig& config, SmartFollower* followerPtr, float danceDuration, float danceSpeed) :
        BT::StatefulActionNode(name, config),
        mSmartFollowerPtr(followerPtr),
        mTimer(danceDuration),
        mDancingSpeed(danceSpeed),
        mDancingDirection(true)
    {
    }

    static BT::PortsList providedPorts()
    {
        return {};
    }

    virtual BT::NodeStatus onStart() override
    {
        mTimer.reset();
        return BT::NodeStatus::RUNNING;
    }

    virtual BT::NodeStatus onRunning() override
    {
        RUNTIME_CHECK(mSmartFollowerPtr);
        const float deltaTime = mSmartFollowerPtr->getDeltaTime();
        mTimer.update(deltaTime);

        float& currentAngle = mSmartFollowerPtr->getBellota().transform().angle();
        
        if (currentAngle >= 45.f)
            mDancingDirection = false;
        else if (currentAngle <= -45.f)
            mDancingDirection = true;

        if (mDancingDirection)
            currentAngle += mDancingSpeed * deltaTime;
        else
            currentAngle -= mDancingSpeed * deltaTime;

        if (mTimer.isFinished())
            return BT::NodeStatus::SUCCESS;
        else
            return BT::NodeStatus::RUNNING;
    }

    virtual void onHalted() override
    {
    }

private:
    SmartFollower* mSmartFollowerPtr;
    Timer mTimer;
    float mDancingSpeed;
    bool mDancingDirection;
};

} // namespace ActionNodes

struct BehaviorTree::Impl
{
    BT::Tree mTree;
    float mDeltaTime;
};

SmartFollower::SmartFollower() :
    mPlayerPtr(nullptr),
    mBehaviorTree(),
    mVelocityComponent() 
{
}

void SmartFollower::init(Nothofagus::Canvas *canvasPtr, Nothofagus::BellotaId bellotaId, float speed, Player *playerPtr, float danceDuration, float danceSpeed)
{
    GameObject::init(canvasPtr, bellotaId);
    mVelocityComponent.init({1.f, 0}, speed);
    mPlayerPtr = playerPtr;

    BT::BehaviorTreeFactory factory;
    factory.registerNodeType<ActionNodes::LookForPlayer>("LookForPlayer", this, playerPtr);
    factory.registerNodeType<ActionNodes::CloseEnough>("CloseEnough", this, 50.f);
    factory.registerNodeType<ActionNodes::MoveTo>("MoveTo", this, 25.f);
    factory.registerNodeType<ActionNodes::Dance>("Dance", this, danceDuration, danceSpeed);

    BT::Tree tree = factory.createTreeFromText(R"(
        <root BTCPP_format="4" >
            <BehaviorTree ID="MainTree">
                <Fallback>
                    <Sequence>
                        <LookForPlayer player_location="{PlayerLocation}"/>
                        <CloseEnough player_location="{PlayerLocation}"/>
                        <Dance/>
                    </Sequence>
                    <MoveTo destination="{PlayerLocation}"/>
                </Fallback>
            </BehaviorTree>
        </root>
    )");

    mBehaviorTree.pImpl = new BehaviorTree::Impl( std::move(tree), 0.f );
}

SmartFollower::~SmartFollower()
{
    delete mBehaviorTree.pImpl;
}

float SmartFollower::getDeltaTime() const
{
    return mBehaviorTree.pImpl->mDeltaTime;
}

void SmartFollower::onCreate()
{
}

void SmartFollower::update(float deltaTime)
{
    RUNTIME_CHECK(mBehaviorTree.pImpl);
    mBehaviorTree.pImpl->mDeltaTime = deltaTime;
    mBehaviorTree.pImpl->mTree.tickOnce();
}

void SmartFollower::onDestroy()
{
}
