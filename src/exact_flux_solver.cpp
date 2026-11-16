#include "exact_flux_solver.hpp"
#include "exact_riemann_solver.hpp"
#include "face_flux.hpp"

#include <stdexcept>

Flux ExactFluxSolver::solve_flux(const PrimitiveState& left, const PrimitiveState& right,
                                 double gamma) const {
    const RiemannResult result = solve_riemann(left, right, gamma);
    if (result.vacuum.occurs) {
        throw std::runtime_error("ExactFluxSolver cannot return a flux for a vacuum solution");
    }
    const RiemannSolution& solution = *result.solution;
    return calculate_face_flux(left, right, gamma, solution.star, solution.waves).flux;
}
