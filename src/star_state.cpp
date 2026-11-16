#include "star_state.hpp"
#include "wave_function.hpp"

#include <cmath>
#include <stdexcept>

namespace {

double star_density(const PrimitiveState& state, double gamma, double pressure) {
    if (pressure > state.p) {
        const double numerator = (gamma + 1.0) * pressure + (gamma - 1.0) * state.p;
        const double denominator = (gamma - 1.0) * pressure + (gamma + 1.0) * state.p;
        return state.rho * numerator / denominator;
    }
    return state.rho * std::pow(pressure / state.p, 1.0 / gamma);
}

} // namespace

StarState calculate_star_state(const PrimitiveState& left, const PrimitiveState& right,
                               double gamma, double pressure) {
    sound_speed(left, gamma);
    sound_speed(right, gamma);
    if (pressure <= 0.0) throw std::invalid_argument("star pressure must be positive");
    StarState result;
    result.p = pressure;
    result.u = (left.u + right.u + wave_function(pressure, right, gamma)
                - wave_function(pressure, left, gamma)) / 2.0;
    result.rho_left = star_density(left, gamma, pressure);
    result.rho_right = star_density(right, gamma, pressure);
    return result;
}
