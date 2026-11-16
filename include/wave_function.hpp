#pragma once

#include "state.hpp"

double sound_speed(const PrimitiveState& state, double gamma);
double wave_function(double pressure, const PrimitiveState& state, double gamma);
double wave_derivative(double pressure, const PrimitiveState& state, double gamma);
