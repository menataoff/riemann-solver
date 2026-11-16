#pragma once

#include "config.hpp"
#include "exact_riemann_solver.hpp"

#include <filesystem>

void write_pu_curves_csv(const std::filesystem::path& directory, const Config& config,
                         const RiemannSolution& solution, int point_count);
