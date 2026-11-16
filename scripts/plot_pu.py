"""Строит волновые кривые на плоскости p–u по сохранённым CSV."""

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


def plot_case(case_dir: Path, figure_dir: Path, number: int | str,
              log_pressure: bool | None = None) -> None:
    metadata = read_metadata(case_dir / "metadata.csv")
    curves = np.genfromtxt(case_dir / "pu_curves.csv", delimiter=",", names=True)
    pressure = curves["p"]

    figure, axis = plt.subplots(figsize=(9, 7))
    for side, name, color in (("left", "Левая", "tab:blue"),
                              ("right", "Правая", "tab:red")):
        initial_p = float(metadata[f"{side}_p"])
        velocity = curves[f"u_{side}"]
        rarefaction = pressure <= initial_p
        shock = pressure >= initial_p
        axis.plot(velocity[rarefaction], pressure[rarefaction], color=color,
                  linestyle="--", linewidth=1.8, label=f"{name} волна разрежения")
        axis.plot(velocity[shock], pressure[shock], color=color,
                  linestyle="-", linewidth=1.8, label=f"{name} ударная ветвь")
        axis.scatter([float(metadata[f"{side}_u"])], [initial_p], color=color,
                     s=55, zorder=4)
        axis.annotate("Л" if side == "left" else "П",
                      (float(metadata[f"{side}_u"]), initial_p),
                      xytext=(5, 5), textcoords="offset points")

    u_star = float(metadata["u_star"])
    p_star = float(metadata["p_star"])
    axis.scatter([u_star], [p_star], color="black", marker="*", s=150,
                 zorder=5, label="Точка пересечения (u*, p*)")
    axis.annotate("*", (u_star, p_star), xytext=(7, 6), textcoords="offset points")
    if log_pressure is None:
        if isinstance(number, int):
            log_pressure = number >= 3
        else:
            pressures = [float(metadata["left_p"]), float(metadata["right_p"]), p_star]
            log_pressure = max(pressures) / min(pressures) >= 1000
    if log_pressure:
        axis.set_yscale("log")
    case_title = f"Тест Торо {number}" if isinstance(number, int) else f"Задача {number}"
    file_stem = f"toro_{number}" if isinstance(number, int) else number
    axis.set(xlabel="Скорость u", ylabel="Давление p",
             title=f"{case_title}: волновые кривые на плоскости p–u")
    axis.grid(alpha=0.2, which="both")
    axis.legend(loc="best", fontsize=9)
    figure.tight_layout()
    figure_dir.mkdir(parents=True, exist_ok=True)
    figure.savefig(figure_dir / f"{file_stem}_pu.png", dpi=180)
    plt.close(figure)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--data-dir", type=Path, default=Path("output/cases"))
    parser.add_argument("--figure-dir", type=Path, default=Path("docs/figures"))
    args = parser.parse_args()
    for number in range(1, 6):
        plot_case(args.data_dir / f"toro_{number}", args.figure_dir, number)
        print(f"Сохранена диаграмма p–u теста Торо {number}")


if __name__ == "__main__":
    main()
