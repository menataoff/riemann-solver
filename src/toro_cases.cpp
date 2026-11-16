#include "toro_cases.hpp"

const std::array<ToroCase, 5>& toro_cases() {
    static const std::array<ToroCase, 5> cases{{
        {1, "Sod", "examples/toro_1.yaml", {0.30313, 0.92745, 0.42632, 0.26557},
         WaveType::Rarefaction, WaveType::Shock},
        {2, "two rarefactions", "examples/toro_2.yaml", {0.00189, 0.0, 0.02185, 0.02185},
         WaveType::Rarefaction, WaveType::Rarefaction},
        {3, "left Woodward-Colella", "examples/toro_3.yaml", {460.894, 19.5975, 0.57506, 5.99924},
         WaveType::Rarefaction, WaveType::Shock},
        {4, "right Woodward-Colella", "examples/toro_4.yaml", {46.0950, -6.19633, 5.99242, 0.57511},
         WaveType::Shock, WaveType::Rarefaction},
        {5, "two strong shocks", "examples/toro_5.yaml", {1691.64, 8.68975, 14.2823, 31.0426},
         WaveType::Shock, WaveType::Shock}
    }};
    return cases;
}
