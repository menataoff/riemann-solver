#pragma once

#include "state.hpp"

enum class GuessMethod {
    Acoustic,
    TwoRarefactions,
    TwoShocks
};

struct PressureResult {
    double pressure = 0.0;
    double initial_pressure = 0.0;
    int iterations = 0;
    GuessMethod guess_method = GuessMethod::Acoustic;
};

const char* guess_method_name(GuessMethod method);
PressureResult solve_pressure(const PrimitiveState& left, const PrimitiveState& right, double gamma);
