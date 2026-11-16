#pragma once

#include "config.hpp"
#include "exact_riemann_solver.hpp"

#include <filesystem>

void write_case_metadata(const std::filesystem::path& directory, const Config& config,
                         const RiemannSolution& solution);
