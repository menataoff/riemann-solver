#pragma once

#include "wave_speeds.hpp"

#include <array>
#include <string>

struct ToroReference {
    double p_star;
    double u_star;
    double rho_star_left;
    double rho_star_right;
};

struct ToroCase {
    int number;
    std::string name;
    std::string config_file;
    ToroReference reference;
    WaveType left_wave;
    WaveType right_wave;
};

const std::array<ToroCase, 5>& toro_cases();
