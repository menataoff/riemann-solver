#include "export_case.hpp"

#include <fstream>
#include <iomanip>
#include <stdexcept>

void write_case_metadata(const std::filesystem::path& directory, const Config& config,
                         const RiemannSolution& solution) {
    std::filesystem::create_directories(directory);
    std::ofstream file(directory / "metadata.csv");
    if (!file) throw std::runtime_error("Cannot write metadata.csv");
    file << std::setprecision(17);
    file << "key,value\n";
    file << "gamma," << config.gamma << '\n';
    file << "x_min," << config.x_min << '\n';
    file << "x_max," << config.x_max << '\n';
    file << "x0," << config.discontinuity << '\n';
    file << "t_out," << config.sample_time << '\n';
    file << "left_rho," << config.left.rho << '\n';
    file << "left_u," << config.left.u << '\n';
    file << "left_p," << config.left.p << '\n';
    file << "right_rho," << config.right.rho << '\n';
    file << "right_u," << config.right.u << '\n';
    file << "right_p," << config.right.p << '\n';
    file << "p_star," << solution.star.p << '\n';
    file << "u_star," << solution.star.u << '\n';
    file << "rho_star_left," << solution.star.rho_left << '\n';
    file << "rho_star_right," << solution.star.rho_right << '\n';
    file << "left_type," << wave_type_name(solution.waves.left.type) << '\n';
    file << "left_speed," << solution.waves.left.speed << '\n';
    file << "left_head," << solution.waves.left.head_speed << '\n';
    file << "left_tail," << solution.waves.left.tail_speed << '\n';
    file << "right_type," << wave_type_name(solution.waves.right.type) << '\n';
    file << "right_speed," << solution.waves.right.speed << '\n';
    file << "right_head," << solution.waves.right.head_speed << '\n';
    file << "right_tail," << solution.waves.right.tail_speed << '\n';
}
