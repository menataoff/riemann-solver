#include "output.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>

Output::Output(const Config& config) {
    if (config.output_destination == "console") {
        stream = &std::cout;
    } else if (config.output_destination == "file") {
        const std::filesystem::path path(config.output_file);
        if (!path.parent_path().empty()) {
            std::filesystem::create_directories(path.parent_path());
        }
        file.open(path);
        if (!file) {
            throw std::runtime_error("Cannot open output file: " + config.output_file);
        }
        stream = &file;
    } else {
        throw std::invalid_argument("output.destination must be console or file");
    }
}

std::ostream& Output::get_stream() {
    return *stream;
}
