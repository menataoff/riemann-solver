#pragma once

#include "state.hpp"

#include <string>

struct BoundaryCondition {
    std::string type = "transmissive";
    PrimitiveState fixed_state;
};

struct Config {
    double gamma = 1.4;
    double x_min = 0.0;
    double x_max = 1.0;
    double discontinuity = 0.5;
    PrimitiveState left;
    PrimitiveState right;
    BoundaryCondition left_boundary;
    BoundaryCondition right_boundary;
    std::string output_destination = "console";
    std::string output_file = "output/run.log";
    double sample_x = 0.5;
    double sample_time = 0.25;
};

Config load_config(const std::string& file_name);
