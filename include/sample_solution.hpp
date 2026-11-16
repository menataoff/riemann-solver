#pragma once

#include "wave_speeds.hpp"

PrimitiveState sample_solution(double s, const PrimitiveState& left, const PrimitiveState& right,
                               double gamma, const StarState& star, const WavePair& waves);
