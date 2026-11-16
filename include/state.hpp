#pragma once

struct PrimitiveState {
    double rho = 0.0;
    double u = 0.0;
    double p = 0.0;
};

struct ConservativeState {
    double rho = 0.0;
    double momentum = 0.0;
    double energy = 0.0;
};

struct Flux {
    double mass = 0.0;
    double momentum = 0.0;
    double energy = 0.0;
};
