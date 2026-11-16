#include "wave_speeds.hpp"
#include "wave_function.hpp"

#include <cmath>

const char* wave_type_name(WaveType type) {
    return type == WaveType::Shock ? "shock" : "rarefaction";
}

WavePair calculate_wave_speeds(const PrimitiveState& left, const PrimitiveState& right,
                               double gamma, const StarState& star) {
    const double a_left = sound_speed(left, gamma);
    const double a_right = sound_speed(right, gamma);
    const double exponent = (gamma - 1.0) / (2.0 * gamma);
    WavePair result;

    if (star.p > left.p) {
        result.left.type = WaveType::Shock;
        result.left.speed = left.u - a_left * std::sqrt(
            (gamma + 1.0) / (2.0 * gamma) * star.p / left.p
            + (gamma - 1.0) / (2.0 * gamma));
    } else {
        result.left.type = WaveType::Rarefaction;
        result.left.head_speed = left.u - a_left;
        result.left.tail_speed = star.u - a_left * std::pow(star.p / left.p, exponent);
    }

    if (star.p > right.p) {
        result.right.type = WaveType::Shock;
        result.right.speed = right.u + a_right * std::sqrt(
            (gamma + 1.0) / (2.0 * gamma) * star.p / right.p
            + (gamma - 1.0) / (2.0 * gamma));
    } else {
        result.right.type = WaveType::Rarefaction;
        result.right.head_speed = right.u + a_right;
        result.right.tail_speed = star.u + a_right * std::pow(star.p / right.p, exponent);
    }
    return result;
}
