#ifndef GUI_DOUBLY_LINKED_LIST_GUI_HPP_
#define GUI_DOUBLY_LINKED_LIST_GUI_HPP_

#include <cstddef>
#include <iostream>

#include "base_gui.hpp"
#include "constants.hpp"
#include "core/doubly_linked_list.hpp"
#include "node_gui.hpp"
#include "raylib.h"
#include "settings.hpp"

namespace gui {

template<typename T>
class GuiDoublyLinkedList : public core::DoublyLinkedList<GuiNode<T>>,
                            public internal::Base {
private:
    using Base = core::DoublyLinkedList<GuiNode<T>>;

    static constexpr Vector2 head_pos{
        constants::scene_width / 2.0F - 15 * GuiNode<T>::radius,
        constants::scene_height / 2.0F};

    using Base::m_head;
    using Base::m_tail;

    void render_link(Vector2 src, Vector2 dest) override;

public:
    using Base::Base;

    using Base::empty;
    using Base::size;

    GuiDoublyLinkedList(std::initializer_list<GuiNode<T>> init_list);

    void insert(std::size_t index, const T& elem);

    void update() override;
    void render() override;
    void init_label();
};

template<typename T>
void GuiDoublyLinkedList<T>::init_label() {
    if (m_head != nullptr) {
        m_head->data.set_label("head");
    }

    if (m_tail != nullptr) {
        if (m_head == m_tail) {
            m_tail->data.set_label("head/tail");
        } else {
            m_tail->data.set_label("tail");
        }
    }
}

template<typename T>
GuiDoublyLinkedList<T>::GuiDoublyLinkedList(
    std::initializer_list<GuiNode<T>> init_list)
    : core::DoublyLinkedList<GuiNode<T>>(init_list) {
    init_label();
}

template<typename T>
void GuiDoublyLinkedList<T>::insert(std::size_t index, const T& elem) {
    Base::insert(index, GuiNode{elem});
}

template<typename T>
void GuiDoublyLinkedList<T>::render_link(Vector2 src, Vector2 dest) {
    constexpr int radius = GuiNode<T>::radius;
    constexpr float scaled_len = radius / 8.0F;

    // straight line
    Vector2 link_pos{src.x + radius, src.y - scaled_len};
    Vector2 link_size{dest.x - src.x - 2 * radius, 2 * scaled_len};

    // right arrow
    constexpr int arrow_size = scaled_len * 5;
    Vector2 right_head{dest.x - radius + scaled_len / 2, src.y};
    Vector2 right_side_top{right_head.x - arrow_size,
                           right_head.y - arrow_size};
    Vector2 right_side_bot{right_head.x - arrow_size,
                           right_head.y + arrow_size};

    // left arrow
    Vector2 left_head{src.x + radius - scaled_len / 2, src.y};
    Vector2 left_side_top{left_head.x + arrow_size, left_head.y - arrow_size};
    Vector2 left_side_bot{left_head.x + arrow_size, left_head.y + arrow_size};

    // draw all
    const Settings& settings = Settings::get_instance();
    DrawRectangleV(link_pos, link_size, settings.get_color(1));
    DrawTriangle(right_head, right_side_top, right_side_bot,
                 settings.get_color(1));
    DrawTriangle(left_head, left_side_bot, left_side_top,
                 settings.get_color(1));
}

template<typename T>
void GuiDoublyLinkedList<T>::render() {
    update();

    for (auto* ptr = m_head; ptr != nullptr; ptr = ptr->next) {
        if (ptr->next != nullptr) {
            render_link(ptr->data.get_pos(), ptr->next->data.get_pos());
        }

        ptr->data.render();
    }
}

template<typename T>
void GuiDoublyLinkedList<T>::update() {
    // TODO: if not outdated then return

    std::size_t pos = 0;

    for (auto* ptr = m_head; ptr != nullptr; ptr = ptr->next) {
        ptr->data.set_pos(
            {head_pos.x + 4 * GuiNode<T>::radius * pos, head_pos.y});
        ++pos;
    }
}

}  // namespace gui

#endif  // GUI_DOUBLY_LINKED_LIST_GUI_HPP_
