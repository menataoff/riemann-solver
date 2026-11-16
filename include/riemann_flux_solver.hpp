#pragma once

#include "state.hpp"

class RiemannFluxSolver {
public:
    virtual ~RiemannFluxSolver() = default;
    virtual Flux solve_flux(const PrimitiveState& left, const PrimitiveState& right,
                            double gamma) const = 0;
};
