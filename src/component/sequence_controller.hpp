#ifndef COMPONENT_SEQUENCE_CONTROLLER_HPP_
#define COMPONENT_SEQUENCE_CONTROLLER_HPP_

#include "raylib.h"

namespace component {

class SequenceController {
private:
    static constexpr Vector2 button_size{25, 25};
    bool m_replay{};
    bool m_prev{};
    bool m_next{};
    int m_progress_value{};
    int m_num_steps{};
    bool m_run_all{};

public:
    void render();
    bool interact();

    void set_max_value(int num);
    void set_progress_value(int value);
    void set_run_all(bool run_all);

    bool get_run_all() const;
    int get_progress_value() const;
};

}  // namespace component

#endif  // COMPONENT_SEQUENCE_CONTROLLER_HPP_