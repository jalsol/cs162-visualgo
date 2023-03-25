#ifndef SCENE_QUEUE_SCENE_HPP_
#define SCENE_QUEUE_SCENE_HPP_

#include <array>

#include "base_scene.hpp"
#include "component/file_dialog.hpp"
#include "component/text_input.hpp"
#include "core/queue.hpp"
#include "gui/queue_gui.hpp"
#include "raygui.h"

namespace scene {

class QueueScene : public internal::BaseScene {
private:
    internal::SceneOptions scene_options{
        // max_size
        8,  // NOLINT

        // mode_labels
        "Mode: Create;"
        "Mode: Push;"
        "Mode: Pop",

        // mode_selection
        0,

        // action_labels
        {
            // Mode: Create
            "Action: Random;"
            "Action: Input;"
            "Action: File",

            // Mode: Push
            "",

            // Mode: Pop
            "",
        },

        // action_selection
        std::vector<int>(3),
    };

    using internal::BaseScene::button_size;
    using internal::BaseScene::head_offset;
    using internal::BaseScene::options_head;

    gui::GuiQueue<int> m_queue{
        gui::GuiNode<int>{1},
        gui::GuiNode<int>{2},
        gui::GuiNode<int>{3},
    };

    bool m_go{};
    component::TextInput m_text_input;
    component::FileDialog m_file_dialog;

    QueueScene() = default;

    using internal::BaseScene::render_go_button;
    using internal::BaseScene::render_options;
    void render_inputs() override;

    //     void render_text_input();
    //     void render_file_input();

    void interact_random();
    void interact_import(core::Deque<int> nums, bool clear,
                         std::size_t amount_to_take);
    void interact_file_import();

public:
    QueueScene(const QueueScene&) = delete;
    QueueScene(QueueScene&&) = delete;
    QueueScene& operator=(const QueueScene&) = delete;
    QueueScene& operator=(QueueScene&&) = delete;
    ~QueueScene() override = default;

    static QueueScene& get_instance();

    void render() override;
    void interact() override;
};

}  // namespace scene

#endif  // SCENE_QUEUE_SCENE_HPP_
