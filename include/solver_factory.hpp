#pragma once

#include "riemann_flux_solver.hpp"

#include <memory>
#include <string>

std::unique_ptr<RiemannFluxSolver> make_solver(const std::string& name);
