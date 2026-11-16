#include "config.hpp"
#include "output.hpp"

#include <exception>
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        const Config config = load_config(argc > 1 ? argv[1] : "config.yaml");
        Output output(config);
        auto& out = output.get_stream();
        out << "Riemann problem: x0=" << config.discontinuity
            << ", gamma=" << config.gamma << '\n';
        out << "left: rho=" << config.left.rho << ", u=" << config.left.u
            << ", p=" << config.left.p << '\n';
        out << "right: rho=" << config.right.rho << ", u=" << config.right.u
            << ", p=" << config.right.p << '\n';
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
