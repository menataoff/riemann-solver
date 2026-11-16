#include "vacuum.hpp"
#include "wave_function.hpp"

VacuumCheck check_vacuum(const PrimitiveState& left, const PrimitiveState& right, double gamma) {
    VacuumCheck result;
    result.velocity_difference = right.u - left.u;
    result.limit = 2.0 * (sound_speed(left, gamma) + sound_speed(right, gamma)) / (gamma - 1.0);
    result.occurs = result.velocity_difference >= result.limit;
    return result;
}
