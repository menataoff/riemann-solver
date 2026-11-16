#include "exact_riemann_solver.hpp"

RiemannResult solve_riemann(const PrimitiveState& left, const PrimitiveState& right, double gamma) {
    RiemannResult result;
    result.vacuum = check_vacuum(left, right, gamma);
    if (result.vacuum.occurs) return result;

    RiemannSolution solution;
    solution.pressure = solve_pressure(left, right, gamma);
    solution.star = calculate_star_state(left, right, gamma, solution.pressure.pressure);
    solution.waves = calculate_wave_speeds(left, right, gamma, solution.star);
    result.solution = solution;
    return result;
}
