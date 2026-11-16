#include "config.hpp"
#include "exact_riemann_solver.hpp"
#include "export_case.hpp"
#include "profile_export.hpp"
#include "pu_export.hpp"
#include "xt_export.hpp"

#include <exception>
#include <filesystem>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: export_case CONFIG.yaml [OUTPUT_DIRECTORY]\n";
        return 1;
    }

    try {
        const Config config = load_config(argv[1]);
        const std::filesystem::path directory = argc == 3 ? argv[2] : "output/custom";
        const RiemannResult result = solve_riemann(config.left, config.right, config.gamma);
        if (result.vacuum.occurs) {
            std::cerr << "Vacuum occurs: u_right - u_left=" << result.vacuum.velocity_difference
                      << " >= limit=" << result.vacuum.limit << '\n';
            std::cerr << "No CSV files were written because a vacuum solution is not implemented.\n";
            return 2;
        }

        write_case_metadata(directory, config, *result.solution);
        write_profile_csv(directory, config, *result.solution, 1001);
        write_xt_density_csv(directory, config, *result.solution, 401, 201);
        write_pu_curves_csv(directory, config, *result.solution, 1001);
        std::cout << "Wrote metadata.csv, profile.csv, xt_density.csv and pu_curves.csv to "
                  << directory << '\n';
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
