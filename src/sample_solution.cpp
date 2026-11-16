#include "sample_solution.hpp"
#include "wave_function.hpp"

#include <cmath>

namespace {

PrimitiveState sample_left_fan(double s, const PrimitiveState& left, double gamma) {
    const double a = sound_speed(left, gamma);
    const double factor = 2.0 / (gamma + 1.0)
                          + (gamma - 1.0) * (left.u - s) / ((gamma + 1.0) * a);
    return {left.rho * std::pow(factor, 2.0 / (gamma - 1.0)),
            2.0 * (a + (gamma - 1.0) * left.u / 2.0 + s) / (gamma + 1.0),
            left.p * std::pow(factor, 2.0 * gamma / (gamma - 1.0))};
}

PrimitiveState sample_right_fan(double s, const PrimitiveState& right, double gamma) {
    const double a = sound_speed(right, gamma);
    const double factor = 2.0 / (gamma + 1.0)
                          - (gamma - 1.0) * (right.u - s) / ((gamma + 1.0) * a);
    return {right.rho * std::pow(factor, 2.0 / (gamma - 1.0)),
            2.0 * (-a + (gamma - 1.0) * right.u / 2.0 + s) / (gamma + 1.0),
            right.p * std::pow(factor, 2.0 * gamma / (gamma - 1.0))};
}

} // namespace

PrimitiveState sample_solution(double s, const PrimitiveState& left, const PrimitiveState& right,
                               double gamma, const StarState& star, const WavePair& waves) {
    if (s <= star.u) {
        const PrimitiveState star_left{star.rho_left, star.u, star.p};
        if (waves.left.type == WaveType::Shock) {
            return s < waves.left.speed ? left : star_left;
        }
        if (s <= waves.left.head_speed) return left;
        if (s >= waves.left.tail_speed) return star_left;
        return sample_left_fan(s, left, gamma);
    }

    const PrimitiveState star_right{star.rho_right, star.u, star.p};
    if (waves.right.type == WaveType::Shock) {
        return s > waves.right.speed ? right : star_right;
    }
    if (s >= waves.right.head_speed) return right;
    if (s <= waves.right.tail_speed) return star_right;
    return sample_right_fan(s, right, gamma);
}
