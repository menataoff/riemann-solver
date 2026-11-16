#include "wave_function.hpp"

#include <cmath>
#include <stdexcept>

double sound_speed(const PrimitiveState& state, double gamma) {
    if (state.rho <= 0.0 || state.p <= 0.0 || gamma <= 1.0) {
        throw std::invalid_argument("rho and p must be positive, gamma must be greater than 1");
    }
    return std::sqrt(gamma * state.p / state.rho);
}

double wave_function(double pressure, const PrimitiveState& state, double gamma) {
    if (pressure <= 0.0) throw std::invalid_argument("pressure must be positive");
    if (pressure > state.p) {
        const double denominator = state.rho * ((gamma + 1.0) * pressure + (gamma - 1.0) * state.p) / 2.0;
        return (pressure - state.p) / std::sqrt(denominator);
    }
    const double exponent = (gamma - 1.0) / (2.0 * gamma);
    return 2.0 * sound_speed(state, gamma) / (gamma - 1.0)
           * (std::pow(pressure / state.p, exponent) - 1.0);
}

double wave_derivative(double pressure, const PrimitiveState& state, double gamma) {
    if (pressure <= 0.0) throw std::invalid_argument("pressure must be positive");
    if (pressure > state.p) {
        const double a = 2.0 / ((gamma + 1.0) * state.rho);
        const double b = (gamma - 1.0) / (gamma + 1.0) * state.p;
        return std::sqrt(a / (pressure + b))
               * (1.0 - (pressure - state.p) / (2.0 * (pressure + b)));
    }
    const double exponent = -(gamma + 1.0) / (2.0 * gamma);
    return std::pow(pressure / state.p, exponent) / (state.rho * sound_speed(state, gamma));
}
