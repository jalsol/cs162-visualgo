#include "linked_list_scene.hpp"

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>

#include "constants.hpp"
#include "raygui.h"
#include "utils.hpp"

namespace scene {

LinkedListScene& LinkedListScene::get_instance() {
    static LinkedListScene scene;
    return scene;
}

void LinkedListScene::render_inputs() {
    int& mode = scene_options.mode_selection;

    switch (mode) {
        case 0: {
            switch (scene_options.action_selection.at(mode)) {
                case 0:
                    break;
                case 1: {
                    m_text_input.render(options_head, head_offset);
                } break;
                case 2: {
                    m_file_dialog.render(options_head, head_offset);
                } break;
                default:
                    utils::unreachable();
            }
        } break;

        case 1: {
            m_index_input.render(options_head, head_offset);
            m_text_input.render(options_head, head_offset);
        } break;

        case 2: {
            m_index_input.render(options_head, head_offset);
        } break;

        case 3: {
            m_index_input.render(options_head, head_offset);
            m_text_input.render(options_head, head_offset);
        } break;

        case 4: {
            m_text_input.render(options_head, head_offset);
        } break;

        default:
            utils::unreachable();
    }

    m_go |= render_go_button();
}

void LinkedListScene::render() {
    m_sequence_controller.inc_anim_counter();

    int frame_idx = m_sequence_controller.get_anim_frame();
    auto* const frame_ptr = m_sequence.find(frame_idx);
    m_sequence_controller.set_progress_value(frame_idx);

    if (frame_ptr != nullptr) {
        frame_ptr->data.render();
    } else {  // end of sequence
        m_list.render();
        m_sequence_controller.set_run_all(false);
    }

    m_sequence_controller.render();
    render_options(scene_options);
}

void LinkedListScene::interact() {
    if (m_sequence_controller.interact()) {
        m_sequence_controller.reset_anim_counter();
        return;
    }

    if (!m_go) {
        return;
    }

    int& mode = scene_options.mode_selection;

    switch (mode) {
        case 0: {
            switch (scene_options.action_selection.at(mode)) {
                case 0: {
                    interact_random();
                } break;

                case 1: {
                    interact_import(m_text_input.extract_values());
                } break;

                case 2: {
                    interact_file_import();
                } break;

                default:
                    utils::unreachable();
            }
        } break;

        case 1: {
            interact_add();
        } break;

        case 2: {
            interact_delete();
        } break;

        case 3: {
            interact_update();
        } break;

        case 4: {
            interact_search();
        } break;

        default:
            utils::unreachable();
    }

    m_go = false;
}

void LinkedListScene::interact_random() {
    std::size_t size =
        utils::get_random(std::size_t{1}, scene_options.max_size);
    m_list = gui::GuiLinkedList<int>();

    for (auto i = 0; i < size; ++i) {
        m_list.insert(
            i, utils::get_random(constants::min_val, constants::max_val));
    }
}

void LinkedListScene::interact_import(core::Deque<int> nums) {
    m_sequence.clear();
    m_list = gui::GuiLinkedList<int>();

    while (!nums.empty()) {
        if (utils::val_in_range(nums.front())) {
            m_list.insert(m_list.size(), nums.front());
        }
        nums.pop_front();
    }
}

void LinkedListScene::interact_file_import() {
    if (!m_file_dialog.is_pressed()) {
        return;
    }

    interact_import(m_file_dialog.extract_values());

    m_file_dialog.reset_pressed();
}

void LinkedListScene::interact_add() {
    int index = m_index_input.extract_values().front();
    int value = m_text_input.extract_values().front();

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

void LinkedListScene::interact_add_head(int value) {
    m_list.insert(0, value);

    m_list.find(0)->data.set_color(ORANGE);
    m_sequence.insert(m_sequence.size(), m_list);

    m_list.find(0)->data.set_color(BLACK);
    m_sequence.insert(m_sequence.size(), m_list);
}

void LinkedListScene::interact_add_tail(int value) {
    std::size_t size = m_list.size();
    m_list.find(size - 1)->data.set_color(GREEN);
    m_sequence.insert(m_sequence.size(), m_list);

    m_list.insert(size, value);
    m_list.find(size)->data.set_color(BLUE);
    m_sequence.insert(m_sequence.size(), m_list);

    m_list.find(size - 1)->data.set_color(BLACK);
    m_list.find(size)->data.set_color(BLACK);
}

void LinkedListScene::interact_add_middle(int index, int value) {
    if (!(0 <= index && index < m_list.size())) {
        return;
    }

    // search until index - 1
    for (int i = 0; i < index - 1; ++i) {
        m_list.find(i)->data.set_color(ORANGE);
        m_sequence.insert(m_sequence.size(), m_list);

        m_list.find(i)->data.set_color(BLACK);
        m_sequence.insert(m_sequence.size(), m_list);
    }

    // reaching index - 1
    // cur
    m_list.find(index - 1)->data.set_color(ORANGE);
    m_sequence.insert(m_sequence.size(), m_list);

    // cur->next
    m_list.find(index)->data.set_color(BLUE);
    m_sequence.insert(m_sequence.size(), m_list);

    // insert between cur and cur->next
    m_list.insert(index, value);
    m_list.find(index)->data.set_color(GREEN);
    m_sequence.insert(m_sequence.size(), m_list);

    // done
    m_list.find(index - 1)->data.set_color(BLACK);
    m_list.find(index)->data.set_color(BLACK);
    m_list.find(index + 1)->data.set_color(BLACK);
}

// void LinkedListScene::interact_push() {
//     if (m_go && m_list.size() < scene_options.max_size) {
//         m_sequence.clear();
//         m_sequence.insert(m_sequence.size(), m_list);

//         auto& old_back = m_list.back();

//         old_back.set_color(GREEN);
//         m_sequence.insert(m_sequence.size(), m_list);

//         m_list.push(m_text_input.extract_values().front());
//         m_list.back().set_color(BLUE);
//         m_sequence.insert(m_sequence.size(), m_list);

//         old_back.set_color(BLACK);
//         m_list.back().set_color(GREEN);
//         m_sequence.insert(m_sequence.size(), m_list);

//         m_list.back().set_color(BLACK);
//         m_sequence.insert(m_sequence.size(), m_list);

//         m_sequence_controller.set_max_value((int)m_sequence.size());
//         m_sequence_controller.set_rerun();
//     }
// }

// void LinkedListScene::interact_pop() {
//     if (m_go && !m_list.empty()) {
//         m_sequence.clear();
//         m_sequence.insert(m_sequence.size(), m_list);

//         m_list.front().set_color(RED);
//         m_sequence.insert(m_sequence.size(), m_list);

//         auto old_front = m_list.front();
//         m_list.pop();

//         if (!m_list.empty()) {
//             m_list.front().set_color(GREEN);
//         }

//         // pls read gui::GuiQueue and core::Queue implementation for
//         push_front m_list.push_front(old_front.get_value());

//         m_list.front().set_color(RED);
//         m_sequence.insert(m_sequence.size(), m_list);

//         m_list.pop();
//         m_sequence.insert(m_sequence.size(), m_list);

//         if (!m_list.empty()) {
//             m_list.front().set_color(BLACK);
//             m_sequence.insert(m_sequence.size(), m_list);
//         }

//         m_sequence_controller.set_max_value((int)m_sequence.size());
//         m_sequence_controller.set_rerun();
//     }
// }

}  // namespace scene
