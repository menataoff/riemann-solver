#include "solver_factory.hpp"
#include "exact_flux_solver.hpp"

#include <stdexcept>

std::unique_ptr<RiemannFluxSolver> make_solver(const std::string& name) {
    if (name == "exact") return std::make_unique<ExactFluxSolver>();
    throw std::invalid_argument("Unknown Riemann solver: " + name);
}
