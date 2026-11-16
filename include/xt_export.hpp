#pragma once

#include "config.hpp"
#include "exact_riemann_solver.hpp"

#include <filesystem>

void write_xt_density_csv(const std::filesystem::path& directory, const Config& config,
                          const RiemannSolution& solution, int x_count, int time_count);
