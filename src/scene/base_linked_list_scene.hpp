#ifndef SCENE_BASE_LINKED_LIST_SCENE_HPP_
#define SCENE_BASE_LINKED_LIST_SCENE_HPP_

#include "base_scene.hpp"
#include "component/code_highlighter.hpp"
#include "component/file_dialog.hpp"
#include "core/doubly_linked_list.hpp"
#include "gui/circular_linked_list_gui.hpp"
#include "gui/doubly_linked_list_gui.hpp"
#include "gui/linked_list_gui.hpp"
#include "raygui.h"

namespace scene {

/**
 * @brief The base linked list scene
 * @tparam Con the container type
 */

template<typename Con>
class BaseLinkedListScene : public internal::BaseScene {
public:
    /**
     * @brief Renders the scene
     */
    void render() override;

    /**
     * @brief Interacts with the scene
     */
    void interact() override;

private:
    /**
     * @brief The scene options
     */
    internal::SceneOptions scene_options{
        // max_size
        8,  // NOLINT

        // mode_labels
        "Mode: Create;"
        "Mode: Add;"
        "Mode: Delete;"
        "Mode: Update;"
        "Mode: Search",

        // mode_selection
        0,

        // action_labels
        {
            // Mode: Create
            "Action: Random;Action: Input;Action: File",
            // Mode: Add
            "",
            // Mode: Delete
            "",
            // Mode: Update
            "",
            // Mode: Search
            "",
        },

        // action_selection
        core::DoublyLinkedList<int>{0, 0, 0, 0, 0},
    };

    /**
     * @brief The mode ids
     */
    enum ModeId {
        Create,
        Add,
        Delete,
        Update,
        Search,
    };

    /**
     * @brief The "Create" action ids
     */
    enum CreateActionId {
        CreateRandom,
        CreateInput,
        CreateFile,
    };

    using internal::BaseScene::button_size;
    using internal::BaseScene::head_offset;
    using internal::BaseScene::options_head;

    /**
     * @brief The container
     */
    Con m_list{
        gui::GuiNode<int>{1},
        gui::GuiNode<int>{2},
        gui::GuiNode<int>{3},
    };

    /**
     * @brief The sequence of containers to be rendered
     */
    core::DoublyLinkedList<Con> m_sequence;

    /**
     * @brief Whether the go button was pressed
     */
    bool m_go{};

    using internal::BaseScene::m_code_highlighter;
    using internal::BaseScene::m_file_dialog;
    using internal::BaseScene::m_index_input;
    using internal::BaseScene::m_sequence_controller;
    using internal::BaseScene::m_text_input;

    using internal::BaseScene::render_go_button;
    using internal::BaseScene::render_options;

    /**
     * @brief Renders the inputs
     */
    void render_inputs() override;

    /**
     * @brief Interacts with random mode
     */
    void interact_random();

    /**
     * @brief Interacts with import mode
     */
    void interact_import(core::Deque<int> nums);

    /**
     * @brief Interacts with file import mode
     */
    void interact_file_import();

    /**
     * @brief Interacts with add mode
     */
    void interact_add();

    /**
     * @brief Interacts with add mode by adding to the head
     */
    void interact_add_head(int value);

    /**
     * @brief Interacts with add mode by adding to the tail
     */
    void interact_add_tail(int value);

    /**
     * @brief Interacts with add mode by adding to the middle
     */
    void interact_add_middle(int index, int value);

    /**
     * @brief Interacts with delete mode
     */
    void interact_delete();

    /**
     * @brief Interacts with delete mode by deleting the head
     */
    void interact_delete_head();

    /**
     * @brief Interacts with delete mode by deleting the tail
     */
    void interact_delete_tail();

    /**
     * @brief Interacts with delete mode by deleting the middle
     */
    void interact_delete_middle(int index);

    /**
     * @brief Interacts with update mode
     */
    void interact_update();

    /**
     * @brief Interacts with search mode
     */
    void interact_search();
};

using LinkedListScene = BaseLinkedListScene<gui::GuiLinkedList<int>>;
using DoublyLinkedListScene =
    BaseLinkedListScene<gui::GuiDoublyLinkedList<int>>;
using CircularLinkedListScene =
    BaseLinkedListScene<gui::GuiCircularLinkedList<int>>;

template<typename Con>
void BaseLinkedListScene<Con>::render_inputs() {
    int& mode = scene_options.mode_selection;

    switch (mode) {
        case Create: {
            switch (scene_options.action_selection.at(mode)) {
                case CreateRandom:
                    break;
                case CreateInput: {
                    m_text_input.render_head(options_head, head_offset);
                } break;
                case CreateFile: {
                    m_go = (m_file_dialog.render_head(options_head,
                                                      head_offset) > 0);
                    return;
                } break;
                default:
                    utils::unreachable();
            }
        } break;

        case Add: {
            m_index_input.render_head(options_head, head_offset);
            m_text_input.render_head(options_head, head_offset);
        } break;

        case Delete: {
            m_index_input.render_head(options_head, head_offset);
        } break;

        case Update: {
            m_index_input.render_head(options_head, head_offset);
            m_text_input.render_head(options_head, head_offset);
        } break;

        case Search: {
            m_text_input.render_head(options_head, head_offset);
        } break;

        default:
            utils::unreachable();
    }

    m_go |= render_go_button();
}

template<typename Con>
void BaseLinkedListScene<Con>::render() {
    m_sequence_controller.inc_anim_counter();

    int frame_idx = m_sequence_controller.get_anim_frame();
    auto* const frame_ptr = m_sequence.find(frame_idx);
    m_sequence_controller.set_progress_value(frame_idx);

    if (frame_ptr != nullptr) {
        frame_ptr->data.render();
        m_code_highlighter.highlight(frame_idx);
    } else {  // end of sequence
        m_list.render();
        m_sequence_controller.set_run_all(false);
    }

    m_code_highlighter.render();
    m_sequence_controller.render();
    render_options(scene_options);
}

template<typename Con>
void BaseLinkedListScene<Con>::interact() {
    if (m_sequence_controller.interact()) {
        m_sequence_controller.reset_anim_counter();
        return;
    }

    m_index_input.set_random_max((int)m_list.size() - 1);

    if (m_text_input.interact() || m_index_input.interact()) {
        return;
    }

    if (!m_go) {
        return;
    }

    int& mode = scene_options.mode_selection;

    switch (mode) {
        case Create: {
            switch (scene_options.action_selection.at(mode)) {
                case CreateRandom: {
                    interact_random();
                } break;

                case CreateInput: {
                    interact_import(m_text_input.extract_values());
                } break;

                case CreateFile: {
                    interact_file_import();
                } break;

                default:
                    utils::unreachable();
            }
            m_code_highlighter.set_code({});
            m_sequence.clear();
            m_sequence_controller.set_max_value(0);
        } break;

        case Add: {
            m_index_input.set_random_max((int)m_list.size());
            interact_add();
        } break;

        case Delete: {
            interact_delete();
        } break;

        case Update: {
            interact_update();
        } break;

        case Search: {
            interact_search();
        } break;

        default:
            utils::unreachable();
    }

    m_go = false;
}

template<typename Con>
void BaseLinkedListScene<Con>::interact_random() {
    std::size_t size =
        utils::get_random(std::size_t{1}, scene_options.max_size);
    m_list = Con();

    for (auto i = 0; i < size; ++i) {
        m_list.insert(
            i, utils::get_random(constants::min_val, constants::max_val));
    }
    m_list.init_label();
}

template<typename Con>
void BaseLinkedListScene<Con>::interact_import(core::Deque<int> nums) {
    m_sequence.clear();
    m_list = Con();

    while (!nums.empty()) {
        if (utils::val_in_range(nums.front())) {
            m_list.insert(m_list.size(), nums.front());
        }
        nums.pop_front();
    }
    m_list.init_label();
}

template<typename Con>
void BaseLinkedListScene<Con>::interact_file_import() {
    interact_import(m_file_dialog.extract_values());
}

template<typename Con>
void BaseLinkedListScene<Con>::interact_add() {
    auto index_container = m_index_input.extract_values();
    if (index_container.empty()) {
        return;
    }

    auto value_container = m_text_input.extract_values();
    if (value_container.empty()) {
        return;
    }

    int index = index_container.front();
    int value = value_container.front();

    if (!(0 <= index && index <= m_list.size())) {
        return;
    }

    if (!utils::val_in_range(value)) {
        return;
    }

    m_sequence.clear();
    m_sequence.insert(m_sequence.size(), m_list);

    if (index == 0) {
        interact_add_head(value);
    } else if (index == m_list.size()) {
        interact_add_tail(value);
    } else {
        interact_add_middle(index, value);
    }

    m_sequence_controller.set_max_value((int)m_sequence.size());
    m_sequence_controller.set_rerun();
}

template<typename Con>
void BaseLinkedListScene<Con>::interact_add_head(int value) {
    m_code_highlighter.set_code({
        "Node* node = new Node(value);",
        "node->next = head;",
        "head = next;",
    });
    m_code_highlighter.push_into_sequence(-1);

    m_list.insert(0, value);

    m_list.at(0).set_color_index(6);
    m_list.at(0).set_label("node");
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(0);

    if (m_list.size() > 1) {
        m_list.at(1).set_color_index(4);
    }

    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(1);

    if (m_list.size() > 1) {
        m_list.at(1).set_color_index(0);
        m_list.at(1).set_label("");
    }

    m_list.at(0).set_color_index(4);
    m_list.at(0).set_label("head");
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(2);

    m_list.at(0).set_color_index(0);
}

template<typename Con>
void BaseLinkedListScene<Con>::interact_add_tail(int value) {
    m_code_highlighter.set_code({
        "Node* node = new Node(value);",
        "tail->next = node;",
        "tail = tail->next;",
    });
    m_code_highlighter.push_into_sequence(-1);

    std::size_t size = m_list.size();

    m_list.insert(size, value);
    m_list.at(size).set_color_index(6);
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(0);

    m_list.at(size - 1).set_color_index(4);
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(1);

    m_list.at(size - 1).set_color_index(0);
    m_list.at(size - 1).set_label("");
    m_list.at(size).set_color_index(4);
    m_list.at(size).set_label("tail");
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(2);

    m_list.at(size).set_color_index(0);
}

template<typename Con>
void BaseLinkedListScene<Con>::interact_add_middle(int index, int value) {
    m_code_highlighter.set_code({
        "Node* pre = head;",
        "for (i = 0; i < index - 1; ++i)",
        "    pre = pre->next;",
        "",
        "Node* nxt = pre->next;",
        "Node* node = new Node(value);",
        "node->next = nxt;",
        "pre->next = node;",
    });
    m_code_highlighter.push_into_sequence(-1);

    m_list.at(0).set_color_index(4);
    m_list.at(0).set_label("head/pre");
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(0);

    // search until index - 1
    for (int i = 0; i < index - 1; ++i) {
        m_list.at(i).set_color_index(2);
        m_sequence.insert(m_sequence.size(), m_list);
        m_code_highlighter.push_into_sequence(1);

        m_list.at(i).set_color_index(0);
        m_list.at(i).set_label(i == 0 ? "head" : "");
        m_list.at(i + 1).set_color_index(2);
        m_list.at(i + 1).set_label("pre");
        m_sequence.insert(m_sequence.size(), m_list);
        m_code_highlighter.push_into_sequence(2);
    }

    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(1);

    // reaching index - 1
    // cur
    m_list.at(index - 1).set_color_index(2);
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(3);

    // cur->next
    m_list.at(index).set_color_index(7);
    m_list.at(index).set_label(index + 1 == m_list.size() ? "tail/nxt" : "nxt");
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(4);

    // insert between cur and cur->next
    m_list.insert(index, value);
    m_list.at(index).set_color_index(6);
    m_list.at(index).set_label("node");
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(5);

    m_list.at(index - 1).set_color_index(2);
    m_list.at(index + 1).set_color_index(0);
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(6);

    m_list.at(index - 1).set_color_index(0);
    m_list.at(index + 1).set_color_index(7);
    m_list.init_label();
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(7);

    // done
    m_list.at(index - 1).set_color_index(0);
    m_list.at(index - 1).set_label("");
    m_list.at(index).set_color_index(0);
    m_list.at(index).set_label("");
    m_list.at(index + 1).set_color_index(0);
    m_list.at(index + 1).set_label("");
    m_list.init_label();
}

template<typename Con>
void BaseLinkedListScene<Con>::interact_delete() {
    if (m_list.empty()) {
        return;
    }

    auto index_container = m_index_input.extract_values();
    if (index_container.empty()) {
        return;
    }

    int index = index_container.front();

    if (!(0 <= index && index < m_list.size())) {
        return;
    }

    m_sequence.clear();
    m_sequence.insert(m_sequence.size(), m_list);

    if (index == 0) {
        interact_delete_head();
    } else if (index + 1 == m_list.size()) {
        interact_delete_tail();
    } else {
        interact_delete_middle(index);
    }

    m_sequence_controller.set_max_value((int)m_sequence.size());
    m_sequence_controller.set_rerun();
}

template<typename Con>
void BaseLinkedListScene<Con>::interact_delete_head() {
    m_code_highlighter.set_code({
        "Node* temp = head;",
        "head = head->next;",
        "delete temp;",
    });
    m_code_highlighter.push_into_sequence(-1);

    m_list.at(0).set_color_index(4);
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(0);

    m_list.at(0).set_color_index(5);
    m_list.at(0).set_label("");
    if (m_list.size() > 1) {
        m_list.at(1).set_color_index(4);
        m_list.at(1).set_label("head");
    }
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(1);

    m_list.remove(0);
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(2);

    if (m_list.size() > 0) {
        m_list.at(0).set_color_index(0);
    }
}

template<typename Con>
void BaseLinkedListScene<Con>::interact_delete_tail() {
    m_code_highlighter.set_code({
        "Node* pre = head;",
        "Node* nxt = pre->next;",
        "while (nxt->next != nullptr)",
        "    pre = pre->next, nxt = nxt->next;",
        "",
        "delete nxt;",
        "tail = pre;",
    });
    m_code_highlighter.push_into_sequence(-1);

    m_list.at(0).set_color_index(2);
    m_list.at(0).set_label("head/pre");
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(0);

    m_list.at(1).set_color_index(3);
    if (m_list.size() == 2) {
        m_list.at(1).set_label("tail/nxt");
    } else {
        m_list.at(1).set_label("nxt");
    }
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(1);

    int idx = 0;
    for (; idx + 2 < m_list.size(); ++idx) {
        m_sequence.insert(m_sequence.size(), m_list);
        m_code_highlighter.push_into_sequence(2);

        m_list.at(idx).set_color_index(0);
        if (idx == 0) {
            m_list.at(idx).set_label("head");
        } else {
            m_list.at(idx).set_label("");
        }

        m_list.at(idx + 1).set_color_index(2);
        m_list.at(idx + 1).set_label("pre");
        m_list.at(idx + 2).set_color_index(3);
        if (idx + 3 == m_list.size()) {
            m_list.at(idx + 2).set_label("tail/nxt");
        } else {
            m_list.at(idx + 2).set_label("nxt");
        }

        m_sequence.insert(m_sequence.size(), m_list);
        m_code_highlighter.push_into_sequence(3);
    }

    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(2);

    m_list.at(idx).set_color_index(2);
    m_list.at(idx).set_label("pre");
    m_list.at(idx + 1).set_color_index(3);
    m_list.at(idx + 1).set_label("tail/nxt");
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(4);

    m_list.remove(idx + 1);
    m_list.at(idx).set_label("tail/pre");
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(5);

    m_list.at(idx).set_color_index(4);
    m_list.init_label();
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(6);

    m_list.at(idx).set_color_index(0);
}

template<typename Con>
void BaseLinkedListScene<Con>::interact_delete_middle(int index) {
    m_code_highlighter.set_code({
        "Node* pre = head;",
        "for (i = 0; i < index - 1; i++)",
        "    pre = pre->next;",
        "",
        "Node* node = pre->next;",
        "Node* nxt = node->next;",
        "delete node;",
        "pre->next = nxt;",
    });
    m_code_highlighter.push_into_sequence(-1);

    m_list.at(0).set_color_index(4);
    m_list.at(0).set_label("head/pre");
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(0);

    int idx = 0;
    for (; idx + 1 < index; ++idx) {
        m_list.at(idx).set_color_index(2);
        m_sequence.insert(m_sequence.size(), m_list);
        m_code_highlighter.push_into_sequence(1);

        m_list.at(idx).set_color_index(0);
        m_list.at(idx).set_label("");
        m_list.at(idx + 1).set_color_index(2);
        m_list.init_label();
        m_list.at(idx + 1).set_label("pre");
        m_sequence.insert(m_sequence.size(), m_list);
        m_code_highlighter.push_into_sequence(2);
    }

    m_list.at(idx).set_color_index(2);
    m_list.at(idx).set_label("pre");
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(3);

    m_list.at(idx + 1).set_color_index(5);
    m_list.at(idx + 1).set_label("node");
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(4);

    m_list.at(idx + 2).set_color_index(3);
    if (idx + 3 == m_list.size()) {
        m_list.at(idx + 2).set_label("tail/nxt");
    } else {
        m_list.at(idx + 2).set_label("nxt");
    }
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(5);

    m_list.remove(idx + 1);
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(6);

    m_list.at(idx + 1).set_color_index(7);
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(7);

    m_list.at(idx).set_color_index(0);
    m_list.at(idx).set_label("");
    m_list.at(idx + 1).set_color_index(0);
    m_list.at(idx + 1).set_label("");
}

template<typename Con>
void BaseLinkedListScene<Con>::interact_update() {
    auto index_container = m_index_input.extract_values();
    if (index_container.empty()) {
        return;
    }

    auto value_container = m_text_input.extract_values();
    if (value_container.empty()) {
        return;
    }

    int index = index_container.front();
    int value = value_container.front();

    if (!(0 <= index && index < m_list.size())) {
        return;
    }

    m_code_highlighter.set_code({
        "Node* node = head;",
        "for (i = 0; i < index; i++)",
        "    node = node->next;",
        "",
        "node->value = value;",
    });

    m_sequence.clear();
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(-1);

    m_list.at(0).set_color_index(4);
    m_list.at(0).set_label("head/node");
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(0);

    for (int i = 0; i < index; ++i) {
        m_list.at(i).set_color_index(2);
        m_sequence.insert(m_sequence.size(), m_list);
        m_code_highlighter.push_into_sequence(1);

        m_list.at(i).set_color_index(0);
        m_list.at(i).set_label(i == 0 ? "head" : "");
        m_list.at(i + 1).set_color_index(2);
        m_list.at(i + 1).set_label(i + 2 == m_list.size() ? "tail/node"
                                                          : "node");
        m_sequence.insert(m_sequence.size(), m_list);
        m_code_highlighter.push_into_sequence(2);
    }

    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(1);
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(3);

    m_list.at(index).set_color_index(3);
    m_list.at(index).set_value(value);
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(4);

    m_list.at(index).set_color_index(0);
    m_list.at(index).set_label("");
    m_list.init_label();

    m_sequence_controller.set_max_value((int)m_sequence.size());
    m_sequence_controller.set_rerun();
}

template<typename Con>
void BaseLinkedListScene<Con>::interact_search() {
    auto value_container = m_text_input.extract_values();
    if (value_container.empty()) {
        return;
    }

    int value = value_container.front();
    if (!utils::val_in_range(value)) {
        return;
    }

    m_code_highlighter.set_code({
        "Node* node = head;",
        "while (node != nullptr) {",
        "    if (node->value == value)",
        "        return node;",
        "    node = node->next;",
        "}",
        "return not_found",
    });

    m_sequence.clear();
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(-1);

    m_list.at(0).set_color_index(4);
    m_list.at(0).set_label("head/node");
    m_sequence.insert(m_sequence.size(), m_list);
    m_code_highlighter.push_into_sequence(0);

    std::size_t idx = 0;

    while (idx < m_list.size()) {
        m_list.at(idx).set_color_index(2);
        m_sequence.insert(m_sequence.size(), m_list);
        m_code_highlighter.push_into_sequence(1);

        m_sequence.insert(m_sequence.size(), m_list);
        m_code_highlighter.push_into_sequence(2);
        if (m_list.at(idx).get_value() == value) {
            m_list.at(idx).set_color_index(3);
            m_sequence.insert(m_sequence.size(), m_list);
            m_code_highlighter.push_into_sequence(3);
            m_list.at(idx).set_color_index(0);
            m_list.at(idx).set_label(idx + 1 == m_list.size() ? "tail" : "");
            break;
        }

        m_list.at(idx).set_color_index(0);
        m_list.at(idx).set_label("");
        m_list.init_label();
        ++idx;
        if (idx < m_list.size()) {
            m_list.at(idx).set_color_index(2);
            m_list.at(idx).set_label(idx + 1 == m_list.size() ? "tail/node"
                                                              : "node");
        }
        m_sequence.insert(m_sequence.size(), m_list);
        m_code_highlighter.push_into_sequence(4);
    }

    if (idx >= m_list.size()) {
        m_sequence.insert(m_sequence.size(), m_list);
        m_code_highlighter.push_into_sequence(1);

        m_sequence.insert(m_sequence.size(), m_list);
        m_code_highlighter.push_into_sequence(5);

        m_sequence.insert(m_sequence.size(), m_list);
        m_code_highlighter.push_into_sequence(6);
    }

    m_sequence_controller.set_max_value((int)m_sequence.size());
    m_sequence_controller.set_rerun();
}

}  // namespace scene

#endif  // SCENE_BASE_LINKED_LIST_SCENE_HPP_
