#include "config.hpp"
#include "exact_riemann_solver.hpp"
#include "sample_solution.hpp"
#include "toro_cases.hpp"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

constexpr double relative_tolerance = 0.005;
constexpr double absolute_tolerance = 1e-5;

double relative_error(double value, double reference) {
    if (reference == 0.0) return 0.0;
    return std::abs(value - reference) / std::abs(reference);
}

bool matches(double value, double reference) {
    return std::isfinite(value)
           && (std::abs(value - reference) <= absolute_tolerance
               || relative_error(value, reference) <= relative_tolerance);
}

void write_value(std::ostream& out, const char* name, double value, double reference) {
    out << "| " << name << " | " << value << " | " << reference << " | ";
    if (reference == 0.0) {
        out << "— (absolute error " << std::abs(value - reference) << ")";
    } else {
        out << 100.0 * relative_error(value, reference) << "%";
    }
    out << " |\n";
}

void write_wave(std::ostream& out, const char* side, const WaveInfo& wave) {
    out << "| " << side << " | " << wave_type_name(wave.type) << " | ";
    if (wave.type == WaveType::Shock) {
        out << "speed = " << wave.speed;
    } else {
        out << "head = " << wave.head_speed << ", tail = " << wave.tail_speed;
    }
    out << " |\n";
}

bool valid_wave(const WaveInfo& wave, bool left) {
    if (wave.type == WaveType::Shock) return std::isfinite(wave.speed);
    return std::isfinite(wave.head_speed) && std::isfinite(wave.tail_speed)
           && (left ? wave.head_speed <= wave.tail_speed
                    : wave.tail_speed <= wave.head_speed);
}

bool run_case(const ToroCase& test, std::ostream* report) {
    const Config config = load_config(test.config_file);
    const RiemannResult result = solve_riemann(config.left, config.right, config.gamma);
    if (result.vacuum.occurs || !result.solution) {
        std::cout << "[FAIL] Toro " << test.number << ": unexpected vacuum\n";
        return false;
    }
    const RiemannSolution& solution = *result.solution;
    const StarState& star = solution.star;
    const WavePair& waves = solution.waves;
    const ToroReference& ref = test.reference;

    const bool values_ok = matches(star.p, ref.p_star)
                           && matches(star.u, ref.u_star)
                           && matches(star.rho_left, ref.rho_star_left)
                           && matches(star.rho_right, ref.rho_star_right);
    const bool waves_ok = waves.left.type == test.left_wave
                          && waves.right.type == test.right_wave
                          && valid_wave(waves.left, true)
                          && valid_wave(waves.right, false);
    const PrimitiveState at_face = sample_solution(0.0, config.left, config.right,
                                                   config.gamma, star, waves);
    const bool sample_ok = std::isfinite(at_face.rho) && std::isfinite(at_face.u)
                           && std::isfinite(at_face.p) && at_face.rho > 0.0 && at_face.p > 0.0;
    const bool passed = values_ok && waves_ok && sample_ok;

    std::cout << (passed ? "[PASS] " : "[FAIL] ") << "Toro " << test.number
              << " (" << test.name << "): p*=" << star.p << ", u*=" << star.u
              << ", guess=" << guess_method_name(solution.pressure.guess_method)
              << ", Newton=" << solution.pressure.iterations << '\n';

    if (report) {
        *report << "## Тест " << test.number << ": " << test.name << "\n\n";
        *report << "Конфигурация: [`" << test.config_file << "`](../" << test.config_file
                << "). Момент вывода: `t = " << config.sample_time << "`. Результат: **"
                << (passed ? "пройден" : "не пройден") << "**.\n\n";
        *report << "| Величина | Расчёт | Таблица задания | Относительное отклонение |\n"
                << "| --- | ---: | ---: | ---: |\n";
        write_value(*report, "p*", star.p, ref.p_star);
        write_value(*report, "u*", star.u, ref.u_star);
        write_value(*report, "rho*L", star.rho_left, ref.rho_star_left);
        write_value(*report, "rho*R", star.rho_right, ref.rho_star_right);
        *report << "\n| Сторона | Тип волны | Скорость / границы веера |\n"
                << "| --- | --- | --- |\n";
        write_wave(*report, "левая", waves.left);
        write_wave(*report, "правая", waves.right);
        *report << "\nНачальное приближение GUESSP: `"
                << guess_method_name(solution.pressure.guess_method) << "`, p^(0) = `"
                << solution.pressure.initial_pressure << "`. Итераций Ньютона: **"
                << solution.pressure.iterations << "**.\n\n";
    }
    return passed;
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        std::ofstream report_file;
        if (argc > 1) {
            const std::filesystem::path path(argv[1]);
            if (!path.parent_path().empty()) std::filesystem::create_directories(path.parent_path());
            report_file.open(path);
            if (!report_file) throw std::runtime_error("Cannot open report file");
            report_file << "# Результаты пяти тестов Торо\n\n"
                        << "Эталонные значения — таблица 4.3, перепечатанная в `docs/prac15.pdf`. "
                        << "Расчёт выполнен точным решателем по §15 лекций. "
                        << "Допуск для проверки — 0.5% относительно табличного значения "
                        << "или 1e-5 абсолютно (для нуля и округлённых малых значений).\n\n";
            report_file << std::setprecision(10);
        }

        bool all_passed = true;
        for (const ToroCase& test : toro_cases()) {
            all_passed = run_case(test, report_file ? &report_file : nullptr) && all_passed;
        }
        std::cout << (all_passed ? "ALL PASSED\n" : "TESTS FAILED\n");
        return all_passed ? 0 : 1;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
