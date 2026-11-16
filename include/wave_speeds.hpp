#pragma once

#include "star_state.hpp"

enum class WaveType {
    Shock,
    Rarefaction
};

struct WaveInfo {
    WaveType type = WaveType::Shock;
    double speed = 0.0;
    double head_speed = 0.0;
    double tail_speed = 0.0;
};

struct WavePair {
    WaveInfo left;
    WaveInfo right;
};

const char* wave_type_name(WaveType type);
WavePair calculate_wave_speeds(const PrimitiveState& left, const PrimitiveState& right,
                               double gamma, const StarState& star);
