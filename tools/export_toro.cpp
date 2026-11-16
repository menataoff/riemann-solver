#include "config.hpp"
#include "exact_riemann_solver.hpp"
#include "export_case.hpp"
#include "profile_export.hpp"
#include "toro_cases.hpp"

#include <exception>
#include <filesystem>
#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[]) {
    try {
        const std::filesystem::path root = argc > 1 ? argv[1] : "output/cases";
        for (const ToroCase& test : toro_cases()) {
            const Config config = load_config(test.config_file);
            const RiemannResult result = solve_riemann(config.left, config.right, config.gamma);
            if (!result.solution) throw std::runtime_error("Vacuum in Toro test " + std::to_string(test.number));
            const auto directory = root / ("toro_" + std::to_string(test.number));
            write_case_metadata(directory, config, *result.solution);
            write_profile_csv(directory, config, *result.solution, 1001);
            std::cout << "Exported Toro " << test.number << " to " << directory << '\n';
        }
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
