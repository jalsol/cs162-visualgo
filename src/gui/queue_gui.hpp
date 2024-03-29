#ifndef GUI_QUEUE_GUI_HPP_
#define GUI_QUEUE_GUI_HPP_

#include <cstddef>
#include <iostream>

#include "base_gui.hpp"
#include "constants.hpp"
#include "core/queue.hpp"
#include "node_gui.hpp"
#include "raylib.h"
#include "settings.hpp"

namespace gui {

/**
 * @brief The GUI queue container
 * @tparam T the type of the elements
 */

template<typename T>
class GuiQueue : public core::Queue<GuiNode<T>>, public internal::Base {
private:
    using Base = core::Queue<GuiNode<T>>;

public:
    using Base::Base;

    using Base::empty;
    using Base::size;

    /**
     * @brief Construct a new Gui Queue object
     * @param init_list The initializer list
     */
    GuiQueue(std::initializer_list<GuiNode<T>> init_list);

    /**
     * @brief Pushes an element to the back of the queue
     * @param elem The element
     */
    void push(const T& elem);

    /**
     * @brief Pops an element from the front of the queue
     */
    void pop();

    /**
     * @brief Pushes an element to the front of the queue
     * @param elem The element
     * @note This is for demonstration purposes only
     */
    void push_front(const T& elem);

    /**
     * @brief Pops an element from the back of the queue
     */
    void pop_back();

    /**
     * @brief Updates the GUI queue
     */
    void update() override;

    /**
     * @brief Renders the GUI queue
     */
    void render() override;

    /**
     * @brief Initializes the labels
     */
    void init_label();

private:
    /**
     * @brief The position of the head
     */
    static constexpr Vector2 head_pos{
        constants::scene_width / 2.0F - 15 * GuiNode<T>::radius,
        constants::scene_height / 2.0F};

    using Base::m_head;
    using Base::m_tail;

    /**
     * @brief Renders the link between two nodes
     * @param src The source node
     * @param dest The destination node
     */
    void render_link(Vector2 src, Vector2 dest) override;
};

template<typename T>
void GuiQueue<T>::init_label() {
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
GuiQueue<T>::GuiQueue(std::initializer_list<GuiNode<T>> init_list)
    : core::Queue<GuiNode<T>>(init_list) {
    init_label();
}

template<typename T>
void GuiQueue<T>::push(const T& elem) {
    Base::push(GuiNode<T>{elem});
}

template<typename T>
void GuiQueue<T>::pop() {
    Base::pop();
}

template<typename T>
void GuiQueue<T>::push_front(const T& elem) {
    Base::push_front(GuiNode<T>{elem});
}

template<typename T>
void GuiQueue<T>::pop_back() {
    Base::pop_back();
}

template<typename T>
void GuiQueue<T>::render_link(Vector2 src, Vector2 dest) {
    constexpr int radius = GuiNode<T>::radius;
    constexpr float scaled_len = radius / 8.0F;

    // straight line
    Vector2 link_pos{src.x + radius, src.y - scaled_len};
    Vector2 link_size{dest.x - src.x - 2 * radius, 2 * scaled_len};

    // arrow
    constexpr int arrow_size = scaled_len * 5;
    Vector2 head{dest.x - radius + scaled_len / 2, src.y};
    Vector2 side_top{head.x - arrow_size, head.y - arrow_size};
    Vector2 side_bot{head.x - arrow_size, head.y + arrow_size};

    // draw both
    DrawRectangleV(link_pos, link_size, Settings::get_instance().get_color(1));
    DrawTriangle(head, side_top, side_bot,
                 Settings::get_instance().get_color(1));
}

template<typename T>
void GuiQueue<T>::render() {
    update();

    for (auto* ptr = m_head; ptr != nullptr; ptr = ptr->next) {
        if (ptr->next != nullptr) {
            render_link(ptr->data.get_pos(), ptr->next->data.get_pos());
        }

        ptr->data.render();
    }
}

template<typename T>
void GuiQueue<T>::update() {
    // TODO: if not outdated then return

    std::size_t pos = 0;

    for (auto* ptr = m_head; ptr != nullptr; ptr = ptr->next) {
        ptr->data.set_pos(
            {head_pos.x + 4 * GuiNode<T>::radius * pos, head_pos.y});
        ++pos;
    }
}

}  // namespace gui

#endif  // GUI_QUEUE_GUI_HPP_
