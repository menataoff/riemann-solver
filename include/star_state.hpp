#pragma once

#include "state.hpp"

struct StarState {
    double p = 0.0;
    double u = 0.0;
    double rho_left = 0.0;
    double rho_right = 0.0;
};

StarState calculate_star_state(const PrimitiveState& left, const PrimitiveState& right,
                               double gamma, double pressure);
