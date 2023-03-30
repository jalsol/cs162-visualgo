#ifndef GUI_DYNAMIC_ARRAY_GUI_HPP_
#define GUI_DYNAMIC_ARRAY_GUI_HPP_

#include <array>
#include <cstddef>
#include <initializer_list>
#include <memory>

#include "base_gui.hpp"
#include "element_gui.hpp"
#include "raylib.h"

namespace gui {

template<typename T>
class GuiDynamicArray : public internal::Base {
private:
    static constexpr Vector2 head_pos{
        constants::sidebar_width +
            (constants::scene_width - constants::sidebar_width) / 2.0F -
            15 * GuiElement<T>::side,
        constants::scene_height / 2.0F};

    std::size_t m_capacity{2};
    std::size_t m_size{};
    GuiElement<T>* m_ptr{nullptr};

    void render_link(Vector2 src, Vector2 dest) override;

public:
    GuiDynamicArray();
    GuiDynamicArray(std::initializer_list<T> init_list);
    GuiDynamicArray(const GuiDynamicArray& other);
    ~GuiDynamicArray() override;

    void update() override;
    void render() override;

    T& operator[](std::size_t idx);
    T operator[](std::size_t idx) const;

    void set_color(std::size_t idx, Color color);
    void realloc(std::size_t capacity);

    std::size_t capacity() const;
    std::size_t size() const;

    void push(const T& value);
    void pop();
};

template<typename T>
void GuiDynamicArray<T>::realloc(std::size_t capacity) {
    if (m_capacity > capacity) {
        return;
    }

    while (m_capacity < capacity) {
        m_capacity *= 2;
    }

    auto* new_ptr = new GuiElement<T>[m_capacity];
    for (auto i = 0; i < m_size; ++i) {
        new_ptr[i] = m_ptr[i];
    }
    for (auto i = m_size; i < m_capacity; ++i) {
        new_ptr[i].set_index(i);
    }

    delete[] m_ptr;
    m_ptr = new_ptr;
}

template<typename T>
GuiDynamicArray<T>::GuiDynamicArray() : m_ptr{new GuiElement<T>[m_capacity]} {
    for (auto i = 0; i < m_capacity; ++i) {
        m_ptr[i].set_index(i);
    }
}

template<typename T>
GuiDynamicArray<T>::GuiDynamicArray(std::initializer_list<T> init_list)
    : m_size{init_list.size()} {
    realloc(m_size);

    for (std::size_t idx = 0; auto elem : init_list) {
        *(m_ptr + idx).set_value(elem);
        *(m_ptr + idx).set_color(BLACK);
    }
}

template<typename T>
GuiDynamicArray<T>::GuiDynamicArray(const GuiDynamicArray<T>& other)
    : m_capacity{other.m_capacity},
      m_size{other.m_size},
      m_ptr{new GuiElement<T>[m_capacity]} {
    for (auto i = 0; i < m_capacity; ++i) {
        m_ptr[i] = other.m_ptr[i];
    }
}

template<typename T>
GuiDynamicArray<T>::~GuiDynamicArray() {
    delete[] m_ptr;
}

template<typename T>
void GuiDynamicArray<T>::render_link(Vector2 src, Vector2 dest) {}

template<typename T>
void GuiDynamicArray<T>::render() {
    update();

    std::size_t idx = 0;

    for (std::size_t i = 0; i < m_capacity; ++i) {
        m_ptr[i].render();
    }
}

template<typename T>
void GuiDynamicArray<T>::update() {
    // TODO: if not outdated then return

    for (std::size_t i = 0; i < m_capacity; ++i) {
        m_ptr[i].set_target_pos(
            {head_pos.x + 4 * GuiElement<T>::side * i, head_pos.y});
    }
}

template<typename T>
T& GuiDynamicArray<T>::operator[](std::size_t idx) {
    return m_ptr[idx].get_value();
}

template<typename T>
T GuiDynamicArray<T>::operator[](std::size_t idx) const {
    return m_ptr[idx].get_value();
}

template<typename T>
void GuiDynamicArray<T>::set_color(std::size_t idx, Color color) {
    m_ptr[idx].set_color(color);
}

template<typename T>
std::size_t GuiDynamicArray<T>::capacity() const {
    return m_capacity;
}

template<typename T>
std::size_t GuiDynamicArray<T>::size() const {
    return m_size;
}

template<typename T>
void GuiDynamicArray<T>::push(const T& value) {
    if (m_size + 1 > m_capacity) {
        realloc(m_size + 1);
    }

    m_ptr[m_size].set_color(BLACK);
    m_ptr[m_size].set_value(value);
    ++m_size;
}

template<typename T>
void GuiDynamicArray<T>::pop() {
    if (m_size >= 1) {
        m_ptr[m_size - 1].set_color(GRAY);
        m_ptr[m_size - 1].set_value(0);
        --m_size;
    }
}

}  // namespace gui

#endif  // GUI_DYNAMIC_ARRAY_GUI_HPP_