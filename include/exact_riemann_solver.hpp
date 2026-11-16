#pragma once

#include "pressure_solver.hpp"
#include "star_state.hpp"
#include "wave_speeds.hpp"
#include "vacuum.hpp"

#include <optional>

struct RiemannSolution {
    PressureResult pressure;
    StarState star;
    WavePair waves;
};

struct RiemannResult {
    VacuumCheck vacuum;
    std::optional<RiemannSolution> solution;
};

RiemannResult solve_riemann(const PrimitiveState& left, const PrimitiveState& right, double gamma);
