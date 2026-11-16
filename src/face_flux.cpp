#include "face_flux.hpp"

FaceResult calculate_face_flux(const PrimitiveState& left, const PrimitiveState& right,
                               double gamma, const StarState& star, const WavePair& waves) {
    FaceResult result;
    result.state = sample_solution(0.0, left, right, gamma, star, waves);
    const double rho = result.state.rho;
    const double u = result.state.u;
    const double p = result.state.p;
    const double total_energy = p / (gamma - 1.0) + rho * u * u / 2.0;
    result.flux.mass = rho * u;
    result.flux.momentum = rho * u * u + p;
    result.flux.energy = (total_energy + p) * u;
    return result;
}
