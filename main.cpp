#include "config.hpp"
#include "output.hpp"
#include "pressure_solver.hpp"
#include "star_state.hpp"
#include "wave_speeds.hpp"
#include "sample_solution.hpp"

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
        const PressureResult pressure = solve_pressure(config.left, config.right, config.gamma);
        out << "p_star=" << pressure.pressure << ", guess=" << pressure.initial_pressure
            << " (" << guess_method_name(pressure.guess_method) << ")"
            << ", Newton iterations=" << pressure.iterations << '\n';
        const StarState star = calculate_star_state(config.left, config.right,
                                                    config.gamma, pressure.pressure);
        out << "u_star=" << star.u << ", rho_star_left=" << star.rho_left
            << ", rho_star_right=" << star.rho_right << '\n';
        const WavePair waves = calculate_wave_speeds(config.left, config.right, config.gamma, star);
        auto print_wave = [&out](const char* side, const WaveInfo& wave) {
            out << side << " wave: " << wave_type_name(wave.type);
            if (wave.type == WaveType::Shock) {
                out << ", speed=" << wave.speed;
            } else {
                out << ", head=" << wave.head_speed << ", tail=" << wave.tail_speed;
            }
            out << '\n';
        };
        print_wave("left", waves.left);
        print_wave("right", waves.right);
        const double s = (config.sample_x - config.discontinuity) / config.sample_time;
        const PrimitiveState sample = sample_solution(s, config.left, config.right,
                                                      config.gamma, star, waves);
        out << "sample at S=" << s << ": rho=" << sample.rho << ", u=" << sample.u
            << ", p=" << sample.p << '\n';
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
