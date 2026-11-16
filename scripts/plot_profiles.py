"""Build six exact-solution profiles from CSV files written by export_toro."""

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
    for side, color in (("left", "tab:blue"), ("right", "tab:red")):
        if metadata[f"{side}_type"] == "shock":
            result.append((x0 + time * float(metadata[f"{side}_speed"]),
                           f"{side} shock", color))
        else:
            result.append((x0 + time * float(metadata[f"{side}_head"]),
                           f"{side} fan head", color))
            result.append((x0 + time * float(metadata[f"{side}_tail"]),
                           f"{side} fan tail", color))
    result.append((x0 + time * float(metadata["u_star"]), "contact", "black"))
    return result


def plot_case(case_dir: Path, figure_dir: Path, number: int) -> None:
    metadata = read_metadata(case_dir / "metadata.csv")
    data = np.genfromtxt(case_dir / "profile.csv", delimiter=",", names=True)
    fields = (
        ("rho", "Density ρ"), ("u", "Velocity u"),
        ("p", "Pressure p"), ("e", "Internal energy e"),
        ("a", "Sound speed a"), ("s", "Entropy function s"),
    )
    figure, axes = plt.subplots(3, 2, figsize=(12, 11), sharex=True)
    for axis, (field, title) in zip(axes.flat, fields):
        axis.plot(data["x"], data[field], color="tab:green", linewidth=1.8)
        for position, label, color in wave_lines(metadata):
            is_contact = label == "contact"
            is_fan = "fan" in label
            axis.axvline(position, color=color, linewidth=1.0,
                         linestyle="--" if is_contact else (":" if is_fan else "-"),
                         alpha=0.75, label=label)
        axis.set_ylabel(title)
        axis.grid(alpha=0.2)
        axis.set_xlim(float(metadata["x_min"]), float(metadata["x_max"]))
    axes[0, 0].legend(loc="best", fontsize=8)
    for axis in axes[-1, :]:
        axis.set_xlabel("x")
    figure.suptitle(f"Toro test {number}: exact profiles at t = {float(metadata['t_out']):g}")
    figure.tight_layout()
    figure_dir.mkdir(parents=True, exist_ok=True)
    figure.savefig(figure_dir / f"toro_{number}_profiles.png", dpi=180)
    plt.close(figure)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--data-dir", type=Path, default=Path("output/cases"))
    parser.add_argument("--figure-dir", type=Path, default=Path("docs/figures"))
    args = parser.parse_args()
    for number in range(1, 6):
        plot_case(args.data_dir / f"toro_{number}", args.figure_dir, number)
        print(f"Saved Toro {number} profiles")


if __name__ == "__main__":
    main()
