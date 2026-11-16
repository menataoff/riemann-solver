#pragma once

#include "config.hpp"

#include <fstream>
#include <ostream>

class Output {
private:
    std::ofstream file;
    std::ostream* stream = nullptr;

public:
    explicit Output(const Config& config);
    std::ostream& get_stream();
};
