
#include "basic_game_engine.h"
#include "player.h"
#include "smart_follower.h"

int main()
{
    Nothofagus::ScreenSize screenSize{150, 100};
    Nothofagus::Canvas canvas(screenSize, "Follow and dance via behavior tree", {0, 0, 0}, 10);

    auto makeCharBellota = [&canvas](std::uint8_t character, glm::vec3 color, glm::vec2 location)
    {
        Nothofagus::ColorPallete pallete{
            {0.0, 0.0, 0.0, 0.0},
            {color.r, color.g, color.b, 1.0}
        };
        Nothofagus::Texture texture({8, 8}, {0.5, 0.5, 0.5, 1.0});
        texture.setPallete(pallete);
        Nothofagus::writeChar(texture, character);
        Nothofagus::TextureId textureId = canvas.addTexture(texture);
        Nothofagus::BellotaId bellotaId = canvas.addBellota({{location}, textureId});
        return bellotaId;
    };

    Nothofagus::BellotaId playerBellotaId = makeCharBellota('O', {1,1,1}, {75.0f, 75.0f});
    Nothofagus::BellotaId botBellotaId = makeCharBellota('X', {1,0,0}, {10.0f, 10.0f});

    // Allocating Memory for our Game Objects
    Gamepad gamepad;
    Player player;
    SmartFollower smartFollower;

    // Initializing our Game Objects
    player.init(&canvas, playerBellotaId, 0.12, &gamepad);
    smartFollower.init(&canvas, botBellotaId, 0.04, &player, 2000.f, .5f);

    // Setting up player controller via the gamepad struct
    Nothofagus::Controller controller;
    registerContinuosAction(controller, { Nothofagus::Key::W, Nothofagus::Key::UP }, gamepad.up);
    registerContinuosAction(controller, { Nothofagus::Key::A, Nothofagus::Key::LEFT }, gamepad.left);
    registerContinuosAction(controller, { Nothofagus::Key::S, Nothofagus::Key::DOWN }, gamepad.down);
    registerContinuosAction(controller, { Nothofagus::Key::D, Nothofagus::Key::RIGHT }, gamepad.right);
    controller.registerAction({Nothofagus::Key::ESCAPE, Nothofagus::DiscreteTrigger::Press}, [&]() { canvas.close(); });
    
    // running our simulation
    auto update = [&](float dt)
    {
        // helper to allow reasonable 30fps debugging.
        if (dt > 33.f)
            dt = 33.f;
        
        player.update(dt);
        smartFollower.update(dt);

        ImGui::Begin("BehaviorTree Demo");
        ImGui::Text("Keys: WASD, arrows, ESC");
        ImGui::End();
    };
    canvas.run(update, controller);
    
    return 0;
}