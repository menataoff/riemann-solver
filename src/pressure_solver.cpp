#include "pressure_solver.hpp"
#include "wave_function.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace {

constexpr double tolerance = 1e-6;
constexpr int max_iterations = 100;

double acoustic_guess(const PrimitiveState& left, const PrimitiveState& right, double gamma) {
    const double z_left = left.rho * sound_speed(left, gamma);
    const double z_right = right.rho * sound_speed(right, gamma);
    return (left.p * z_right + right.p * z_left
            + (left.u - right.u) * z_left * z_right) / (z_left + z_right);
}

PressureResult initial_guess(const PrimitiveState& left, const PrimitiveState& right, double gamma) {
    PressureResult result;
    const double p_linear = acoustic_guess(left, right, gamma);
    const double p_min = std::min(left.p, right.p);
    const double p_max = std::max(left.p, right.p);

    if (p_max / p_min <= 2.0 && p_linear >= p_min && p_linear <= p_max) {
        result.initial_pressure = p_linear;
        result.guess_method = GuessMethod::Acoustic;
    } else if (p_linear < p_min) {
        const double exponent = (gamma - 1.0) / (2.0 * gamma);
        const double numerator = sound_speed(left, gamma) + sound_speed(right, gamma)
                                 - (gamma - 1.0) * (right.u - left.u) / 2.0;
        const double denominator = sound_speed(left, gamma) / std::pow(left.p, exponent)
                                   + sound_speed(right, gamma) / std::pow(right.p, exponent);
        if (numerator <= 0.0) throw std::runtime_error("No positive pressure root (possible vacuum)");
        result.initial_pressure = std::pow(numerator / denominator, 1.0 / exponent);
        result.guess_method = GuessMethod::TwoRarefactions;
    } else {
        const double reference = std::max(p_linear, std::numeric_limits<double>::min());
        const double mass_left = std::sqrt(left.rho * ((gamma + 1.0) * reference + (gamma - 1.0) * left.p) / 2.0);
        const double mass_right = std::sqrt(right.rho * ((gamma + 1.0) * reference + (gamma - 1.0) * right.p) / 2.0);
        result.initial_pressure = (mass_left * mass_right * (left.u - right.u)
                                   + mass_right * left.p + mass_left * right.p) / (mass_left + mass_right);
        result.guess_method = GuessMethod::TwoShocks;
    }
    if (!std::isfinite(result.initial_pressure) || result.initial_pressure <= 0.0) {
        throw std::runtime_error("Positive star pressure is too small to represent");
    }
    return result;
}

} // namespace

const char* guess_method_name(GuessMethod method) {
    switch (method) {
        case GuessMethod::Acoustic: return "acoustic";
        case GuessMethod::TwoRarefactions: return "two_rarefactions";
        case GuessMethod::TwoShocks: return "two_shocks";
    }
    return "unknown";
}

PressureResult solve_pressure(const PrimitiveState& left, const PrimitiveState& right, double gamma) {
    sound_speed(left, gamma);
    sound_speed(right, gamma);
    PressureResult result = initial_guess(left, right, gamma);
    double pressure = result.initial_pressure;

    for (int i = 1; i <= max_iterations; ++i) {
        const double value = wave_function(pressure, left, gamma)
                             + wave_function(pressure, right, gamma) + right.u - left.u;
        const double derivative = wave_derivative(pressure, left, gamma)
                                  + wave_derivative(pressure, right, gamma);
        double next = pressure - value / derivative;
        if (!std::isfinite(next)) throw std::runtime_error("Newton iteration produced a non-finite pressure");
        if (next <= 0.0) next = pressure / 2.0;
        if (next <= 0.0) throw std::runtime_error("Positive star pressure is too small to represent");
        result.iterations = i;
        if (2.0 * std::abs(next - pressure) / (next + pressure) < tolerance) {
            result.pressure = next;
            return result;
        }
        pressure = next;
    }
    throw std::runtime_error("Newton iteration did not converge in 100 steps");
}
