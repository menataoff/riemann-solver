#include "profile_export.hpp"
#include "sample_solution.hpp"
#include "wave_function.hpp"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <stdexcept>

void write_profile_csv(const std::filesystem::path& directory, const Config& config,
                       const RiemannSolution& solution, int point_count) {
    if (point_count < 1000) throw std::invalid_argument("profile needs at least 1000 points");
    std::filesystem::create_directories(directory);
    std::ofstream file(directory / "profile.csv");
    if (!file) throw std::runtime_error("Cannot write profile.csv");
    file << std::setprecision(17);
    file << "x,rho,u,p,e,a,s\n";

    for (int i = 0; i < point_count; ++i) {
        const double x = config.x_min + (config.x_max - config.x_min)
                         * static_cast<double>(i) / static_cast<double>(point_count - 1);
        const double ray = (x - config.discontinuity) / config.sample_time;
        const PrimitiveState state = sample_solution(ray, config.left, config.right,
                                                     config.gamma, solution.star, solution.waves);
        const double e = state.p / ((config.gamma - 1.0) * state.rho);
        const double a = sound_speed(state, config.gamma);
        const double entropy = state.p / std::pow(state.rho, config.gamma);
        file << x << ',' << state.rho << ',' << state.u << ',' << state.p << ','
             << e << ',' << a << ',' << entropy << '\n';
    }
}
