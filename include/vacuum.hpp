#pragma once

#include "state.hpp"

struct VacuumCheck {
    bool occurs = false;
    double velocity_difference = 0.0;
    double limit = 0.0;
};

VacuumCheck check_vacuum(const PrimitiveState& left, const PrimitiveState& right, double gamma);
