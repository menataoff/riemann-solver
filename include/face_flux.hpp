#pragma once

#include "sample_solution.hpp"

struct FaceResult {
    PrimitiveState state;
    Flux flux;
};

FaceResult calculate_face_flux(const PrimitiveState& left, const PrimitiveState& right,
                               double gamma, const StarState& star, const WavePair& waves);
