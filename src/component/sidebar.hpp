#ifndef COMPONENT_SIDEBAR_HPP_
#define COMPONENT_SIDEBAR_HPP_

#include <array>

#include "constants.hpp"

namespace component {

class SideBar {
private:
    static constexpr int num_scenes = 8;
    // static constexpr std::array<const char*, num_scenes> scene_names = {{
    //     "Menu",
    //     "Static Array",
    //     "Dynamic Array",
    //     "Linked List",
    //     "Doubly Linked List",
    //     "Circular Linked List",
    //     "Stack",
    //     "Queue",
    // }};

    static constexpr int sidebar_width = constants::sidebar_width;
    static constexpr int button_height = 50;
    // static constexpr int sidebar_height = 50;

    static constexpr const char* sidebar_labels =
        "Back to Menu\n"
        "Static Array\n"
        "Dynamic Array\n"
        "Linked List\n"
        "Doubly Linked List\n"
        "Circular Linked List\n"
        "Stack\n"
        "Queue";

    int m_next_scene{};

public:
    void render();
    void interact();
};

}  // namespace component

#endif  // COMPONENT_SIDEBAR_HPP_
