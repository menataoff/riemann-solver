"""Строит шесть профилей точного решения по сохранённым CSV."""

from pathlib import Path
import argparse
import csv

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np


def read_metadata(path: Path) -> dict[str, str]:
    with path.open(newline="", encoding="utf-8") as file:
        return {row["key"]: row["value"] for row in csv.DictReader(file)}


def wave_lines(metadata: dict[str, str]) -> list[tuple[float, str, str]]:
    x0 = float(metadata["x0"])
    time = float(metadata["t_out"])
    result = []
    for side, wave_name, fan_name, color in (
        ("left", "Левая", "левого", "tab:blue"),
        ("right", "Правая", "правого", "tab:red"),
    ):
        if metadata[f"{side}_type"] == "shock":
            result.append((x0 + time * float(metadata[f"{side}_speed"]),
                           f"{wave_name} ударная волна", color))
        else:
            result.append((x0 + time * float(metadata[f"{side}_head"]),
                           f"Голова {fan_name} веера", color))
            result.append((x0 + time * float(metadata[f"{side}_tail"]),
                           f"Хвост {fan_name} веера", color))
    result.append((x0 + time * float(metadata["u_star"]), "Контактный разрыв", "black"))
    return result


def plot_case(case_dir: Path, figure_dir: Path, number: int | str) -> None:
    metadata = read_metadata(case_dir / "metadata.csv")
    data = np.genfromtxt(case_dir / "profile.csv", delimiter=",", names=True)
    fields = (
        ("rho", "Плотность ρ"), ("u", "Скорость u"),
        ("p", "Давление p"), ("e", "Удельная внутренняя энергия e"),
        ("a", "Скорость звука a"), ("s", "Функция энтропии s"),
    )
    figure, axes = plt.subplots(3, 2, figsize=(12, 11), sharex=True)
    for axis, (field, title) in zip(axes.flat, fields):
        axis.plot(data["x"], data[field], color="tab:green", linewidth=1.8)
        for position, label, color in wave_lines(metadata):
            is_contact = label == "Контактный разрыв"
            is_fan = "веера" in label
            axis.axvline(position, color=color, linewidth=1.0,
                         linestyle="--" if is_contact else (":" if is_fan else "-"),
                         alpha=0.75, label=label)
        axis.set_ylabel(title)
        axis.grid(alpha=0.2)
        axis.set_xlim(float(metadata["x_min"]), float(metadata["x_max"]))
    axes[0, 0].legend(loc="best", fontsize=8)
    for axis in axes[-1, :]:
        axis.set_xlabel("x")
    case_title = f"Тест Торо {number}" if isinstance(number, int) else f"Задача {number}"
    file_stem = f"toro_{number}" if isinstance(number, int) else number
    figure.suptitle(f"{case_title}: профили точного решения при t = {float(metadata['t_out']):g}")
    figure.tight_layout()
    figure_dir.mkdir(parents=True, exist_ok=True)
    figure.savefig(figure_dir / f"{file_stem}_profiles.png", dpi=180)
    plt.close(figure)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--data-dir", type=Path, default=Path("output/cases"))
    parser.add_argument("--figure-dir", type=Path, default=Path("docs/figures"))
    args = parser.parse_args()
    for number in range(1, 6):
        plot_case(args.data_dir / f"toro_{number}", args.figure_dir, number)
        print(f"Сохранены профили теста Торо {number}")


if __name__ == "__main__":
    main()
