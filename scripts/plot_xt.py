"""Строит диаграммы плотности на плоскости x–t по сохранённым CSV."""

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


def draw_wave(axis, x0: float, times: np.ndarray, speed: float,
              color: str, style: str, label: str) -> None:
    axis.plot(x0 + speed * times, times, color=color, linestyle=style,
              linewidth=1.5, label=label)


def wave_boundaries(metadata: dict[str, str], side: str) -> tuple[float, float]:
    if metadata[f"{side}_type"] == "shock":
        speed = float(metadata[f"{side}_speed"])
        return speed, speed
    return float(metadata[f"{side}_head"]), float(metadata[f"{side}_tail"])


def plot_case(case_dir: Path, figure_dir: Path, number: int | str) -> None:
    metadata = read_metadata(case_dir / "metadata.csv")
    raw = np.genfromtxt(case_dir / "xt_density.csv", delimiter=",", names=True)
    x = np.unique(raw["x"])
    times = np.unique(raw["t"])
    rho = raw["rho"].reshape(len(times), len(x))
    x0 = float(metadata["x0"])
    t_out = float(metadata["t_out"])
    contact = float(metadata["u_star"])

    figure, axis = plt.subplots(figsize=(10, 7))
    mesh = axis.pcolormesh(x, times, rho, shading="auto", cmap="viridis")
    figure.colorbar(mesh, ax=axis, label="Плотность ρ")

    for side, wave_name, fan_name, color in (
        ("left", "Левая", "левого", "tab:red"),
        ("right", "Правая", "правого", "tab:orange"),
    ):
        if metadata[f"{side}_type"] == "shock":
            draw_wave(axis, x0, times, float(metadata[f"{side}_speed"]),
                      color, "-", f"{wave_name} ударная волна")
        else:
            head = float(metadata[f"{side}_head"])
            tail = float(metadata[f"{side}_tail"])
            draw_wave(axis, x0, times, head, color, "-", f"Голова {fan_name} веера")
            draw_wave(axis, x0, times, tail, color, "-", f"Хвост {fan_name} веера")
            for speed in np.linspace(head, tail, 7)[1:-1]:
                draw_wave(axis, x0, times, float(speed), color, ":", "_nolegend_")

    draw_wave(axis, x0, times, contact, "white", "--", "Контактный разрыв")

    # Обозначения областей ставятся между волнами при 70 % конечного времени.
    label_time = 0.7 * t_out
    left_head, left_tail = wave_boundaries(metadata, "left")
    right_head, right_tail = wave_boundaries(metadata, "right")
    x_min = float(metadata["x_min"])
    x_max = float(metadata["x_max"])
    regions = (
        ("W_L", x_min, x0 + left_head * label_time),
        ("W_*L", x0 + left_tail * label_time, x0 + contact * label_time),
        ("W_*R", x0 + contact * label_time, x0 + right_tail * label_time),
        ("W_R", x0 + right_head * label_time, x_max),
    )
    for label, start, end in regions:
        center = (start + end) / 2.0
        if x_min <= center <= x_max:
            axis.text(center, label_time, label, ha="center", va="center",
                      color="white", fontsize=10, fontweight="bold",
                      bbox={"facecolor": "black", "alpha": 0.45, "pad": 2})

    case_title = f"Тест Торо {number}" if isinstance(number, int) else f"Задача {number}"
    file_stem = f"toro_{number}" if isinstance(number, int) else number
    axis.set(xlim=(x_min, x_max), ylim=(0, t_out), xlabel="x", ylabel="t",
             title=f"{case_title}: плотность на плоскости x–t")
    axis.legend(loc="upper right", fontsize=8)
    figure.tight_layout()
    figure_dir.mkdir(parents=True, exist_ok=True)
    figure.savefig(figure_dir / f"{file_stem}_xt.png", dpi=180)
    plt.close(figure)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--data-dir", type=Path, default=Path("output/cases"))
    parser.add_argument("--figure-dir", type=Path, default=Path("docs/figures"))
    args = parser.parse_args()
    for number in range(1, 6):
        plot_case(args.data_dir / f"toro_{number}", args.figure_dir, number)
        print(f"Сохранена диаграмма x–t теста Торо {number}")


if __name__ == "__main__":
    main()
