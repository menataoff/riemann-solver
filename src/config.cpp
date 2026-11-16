#include "config.hpp"

#include <cmath>
#include <fstream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::string trim(const std::string& text) {
    const auto first = text.find_first_not_of(" \t\r");
    if (first == std::string::npos) return "";
    const auto last = text.find_last_not_of(" \t\r");
    return text.substr(first, last - first + 1);
}

double number(const std::map<std::string, std::string>& values, const std::string& key) {
    auto it = values.find(key);
    if (it == values.end()) throw std::runtime_error("Missing config key: " + key);
    try {
        size_t used = 0;
        const double value = std::stod(it->second, &used);
        if (used != it->second.size() || !std::isfinite(value)) throw std::invalid_argument("not finite");
        return value;
    } catch (const std::exception&) {
        throw std::runtime_error("Invalid number at config key: " + key);
    }
}

std::string required(const std::map<std::string, std::string>& values, const std::string& key) {
    auto it = values.find(key);
    if (it == values.end() || it->second.empty()) throw std::runtime_error("Missing config key: " + key);
    return it->second;
}

PrimitiveState state(const std::map<std::string, std::string>& values, const std::string& prefix) {
    PrimitiveState result{number(values, prefix + ".rho"),
                          number(values, prefix + ".u"),
                          number(values, prefix + ".p")};
    if (result.rho <= 0.0 || result.p <= 0.0) {
        throw std::runtime_error(prefix + ": rho and p must be positive");
    }
    return result;
}

BoundaryCondition boundary(const std::map<std::string, std::string>& values, const std::string& prefix) {
    BoundaryCondition result;
    result.type = required(values, prefix + ".type");
    if (result.type == "fixed") {
        result.fixed_state = state(values, prefix);
    } else if (result.type != "transmissive") {
        throw std::runtime_error(prefix + ".type must be transmissive or fixed");
    }
    return result;
}

} // namespace

Config load_config(const std::string& file_name) {
    std::ifstream file(file_name);
    if (!file) throw std::runtime_error("Cannot open config file: " + file_name);

    std::map<std::string, std::string> values;
    std::vector<std::string> sections;
    std::string line;
    int line_number = 0;

    while (std::getline(file, line)) {
        ++line_number;
        const auto comment = line.find('#');
        if (comment != std::string::npos) line.erase(comment);
        if (trim(line).empty()) continue;

        const auto first = line.find_first_not_of(' ');
        if (first == std::string::npos || first % 2 != 0 || line.find('\t') != std::string::npos) {
            throw std::runtime_error("Use two-space indentation at line " + std::to_string(line_number));
        }
        const size_t depth = first / 2;
        const std::string entry = trim(line);
        const auto colon = entry.find(':');
        if (colon == std::string::npos || colon == 0) {
            throw std::runtime_error("Expected key: value at line " + std::to_string(line_number));
        }
        const std::string key = trim(entry.substr(0, colon));
        const std::string value = trim(entry.substr(colon + 1));
        if (depth > sections.size()) {
            throw std::runtime_error("Unexpected indentation at line " + std::to_string(line_number));
        }
        sections.resize(depth);
        if (value.empty()) {
            sections.push_back(key);
            continue;
        }
        std::string full_key;
        for (const auto& section : sections) full_key += section + ".";
        full_key += key;
        if (!values.emplace(full_key, value).second) {
            throw std::runtime_error("Duplicate config key: " + full_key);
        }
    }

    Config config;
    config.gamma = number(values, "gas.gamma");
    config.solver_name = required(values, "method.solver");
    config.scheme_name = required(values, "method.scheme");
    config.x_min = number(values, "domain.x_min");
    config.x_max = number(values, "domain.x_max");
    config.discontinuity = number(values, "domain.discontinuity");
    config.left = state(values, "initial.left");
    config.right = state(values, "initial.right");
    config.left_boundary = boundary(values, "boundary.left");
    config.right_boundary = boundary(values, "boundary.right");
    config.output_destination = required(values, "output.destination");
    config.output_file = required(values, "output.file");
    config.sample_x = number(values, "output.sample_x");
    config.sample_time = number(values, "output.sample_time");

    if (config.gamma <= 1.0) throw std::runtime_error("gas.gamma must be greater than 1");
    if (config.solver_name != "exact") throw std::runtime_error("method.solver: only exact is available");
    if (config.scheme_name != "none") throw std::runtime_error("method.scheme: only none is available in part 1");
    if (!(config.x_min < config.discontinuity && config.discontinuity < config.x_max)) {
        throw std::runtime_error("domain.discontinuity must lie inside the domain");
    }
    if (config.sample_x < config.x_min || config.sample_x > config.x_max) {
        throw std::runtime_error("output.sample_x must lie inside the domain");
    }
    if (config.sample_time <= 0.0) throw std::runtime_error("output.sample_time must be positive");
    if (config.output_destination != "console" && config.output_destination != "file") {
        throw std::runtime_error("output.destination must be console or file");
    }
    const std::set<std::string> known_keys{
        "gas.gamma", "method.solver", "method.scheme",
        "domain.x_min", "domain.x_max", "domain.discontinuity",
        "initial.left.rho", "initial.left.u", "initial.left.p",
        "initial.right.rho", "initial.right.u", "initial.right.p",
        "boundary.left.type", "boundary.left.rho", "boundary.left.u", "boundary.left.p",
        "boundary.right.type", "boundary.right.rho", "boundary.right.u", "boundary.right.p",
        "output.destination", "output.file", "output.sample_x", "output.sample_time"
    };
    for (const auto& entry : values) {
        if (known_keys.count(entry.first) == 0) {
            throw std::runtime_error("Unknown config key: " + entry.first);
        }
    }
    return config;
}
