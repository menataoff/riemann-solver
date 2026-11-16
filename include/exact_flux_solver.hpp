#pragma once

#include "riemann_flux_solver.hpp"

class ExactFluxSolver : public RiemannFluxSolver {
public:
    Flux solve_flux(const PrimitiveState& left, const PrimitiveState& right,
                    double gamma) const override;
};
