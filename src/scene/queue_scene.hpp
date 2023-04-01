#ifndef SCENE_QUEUE_SCENE_HPP_
#define SCENE_QUEUE_SCENE_HPP_

#include <array>

#include "base_scene.hpp"
#include "component/file_dialog.hpp"
#include "component/text_input.hpp"
#include "core/doubly_linked_list.hpp"
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
        core::DoublyLinkedList<int>{0, 0, 0},
    };

    using internal::BaseScene::button_size;
    using internal::BaseScene::head_offset;
    using internal::BaseScene::options_head;

    gui::GuiQueue<int> m_queue{
        gui::GuiNode<int>{1},
        gui::GuiNode<int>{2},
        gui::GuiNode<int>{3},
    };
    core::DoublyLinkedList<gui::GuiQueue<int>> m_sequence;

    bool m_go{};
    using internal::BaseScene::m_code_highlighter;
    using internal::BaseScene::m_file_dialog;
    using internal::BaseScene::m_sequence_controller;
    using internal::BaseScene::m_text_input;

    QueueScene() = default;

    using internal::BaseScene::render_go_button;
    using internal::BaseScene::render_options;
    void render_inputs() override;

    void interact_random();
    void interact_import(core::Deque<int> nums);
    void interact_file_import();
    void interact_push();
    void interact_pop();

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
