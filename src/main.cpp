#include "component/sidebar.hpp"
#include "constants.hpp"
#include "raygui.h"
#include "scene/scene_registry.hpp"

#include <iostream>

int main() {
    InitWindow(constants::scene_width, constants::scene_height,
               "VisuAlgo.net clone in C++ by @jalsol");
    SetTargetFPS(constants::frames_per_second);

    GuiLoadStyle("data/bluish_open_sans.rgs");

    scene::SceneRegistry& registry = scene::SceneRegistry::get_instance();
    component::SideBar sidebar;

    while (true) {
        bool should_close = registry.should_close() || WindowShouldClose();
        if (should_close) {
            break;
        }

        if (registry.get_scene() != scene::Menu) {
            sidebar.interact();
        }
        registry.interact();

        BeginDrawing();
        {
            ClearBackground(WHITE);

            if (registry.get_scene() != scene::Menu) {
                sidebar.render();
            }
            registry.render();
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
