#include "pu_export.hpp"
#include "wave_function.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <stdexcept>

void write_pu_curves_csv(const std::filesystem::path& directory, const Config& config,
                         const RiemannSolution& solution, int point_count) {
    if (point_count < 2) throw std::invalid_argument("p-u curves need at least two points");
    std::filesystem::create_directories(directory);
    std::ofstream file(directory / "pu_curves.csv");
    if (!file) throw std::runtime_error("Cannot write pu_curves.csv");
    file << std::setprecision(17);
    file << "p,u_left,u_right\n";

    const double p_min = 0.5 * std::min({config.left.p, config.right.p, solution.star.p});
    const double p_max = 1.5 * std::max({config.left.p, config.right.p, solution.star.p});
    const double log_min = std::log(p_min);
    const double log_max = std::log(p_max);
    for (int i = 0; i < point_count; ++i) {
        const double fraction = static_cast<double>(i) / static_cast<double>(point_count - 1);
        const double pressure = std::exp(log_min + fraction * (log_max - log_min));
        const double u_left = config.left.u - wave_function(pressure, config.left, config.gamma);
        const double u_right = config.right.u + wave_function(pressure, config.right, config.gamma);
        file << pressure << ',' << u_left << ',' << u_right << '\n';
    }
}
