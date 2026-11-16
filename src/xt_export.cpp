#include "xt_export.hpp"
#include "sample_solution.hpp"

#include <fstream>
#include <iomanip>
#include <stdexcept>

void write_xt_density_csv(const std::filesystem::path& directory, const Config& config,
                          const RiemannSolution& solution, int x_count, int time_count) {
    if (x_count < 2 || time_count < 2) throw std::invalid_argument("x-t grid needs at least two points per axis");
    std::filesystem::create_directories(directory);
    std::ofstream file(directory / "xt_density.csv");
    if (!file) throw std::runtime_error("Cannot write xt_density.csv");
    file << std::setprecision(17);
    file << "t,x,rho\n";

    for (int j = 0; j < time_count; ++j) {
        const double time = config.sample_time * static_cast<double>(j)
                            / static_cast<double>(time_count - 1);
        for (int i = 0; i < x_count; ++i) {
            const double x = config.x_min + (config.x_max - config.x_min)
                             * static_cast<double>(i) / static_cast<double>(x_count - 1);
            double rho;
            if (j == 0) {
                rho = x <= config.discontinuity ? config.left.rho : config.right.rho;
            } else {
                const double ray = (x - config.discontinuity) / time;
                rho = sample_solution(ray, config.left, config.right, config.gamma,
                                      solution.star, solution.waves).rho;
            }
            file << time << ',' << x << ',' << rho << '\n';
        }
    }
}
