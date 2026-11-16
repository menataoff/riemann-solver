"""Build p-u wave-curve diagrams from CSV files written by export_toro."""

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


def plot_case(case_dir: Path, figure_dir: Path, number: int) -> None:
    metadata = read_metadata(case_dir / "metadata.csv")
    curves = np.genfromtxt(case_dir / "pu_curves.csv", delimiter=",", names=True)
    pressure = curves["p"]

    figure, axis = plt.subplots(figsize=(9, 7))
    for side, color in (("left", "tab:blue"), ("right", "tab:red")):
        initial_p = float(metadata[f"{side}_p"])
        velocity = curves[f"u_{side}"]
        rarefaction = pressure <= initial_p
        shock = pressure >= initial_p
        axis.plot(velocity[rarefaction], pressure[rarefaction], color=color,
                  linestyle="--", linewidth=1.8, label=f"{side} rarefaction")
        axis.plot(velocity[shock], pressure[shock], color=color,
                  linestyle="-", linewidth=1.8, label=f"{side} shock")
        axis.scatter([float(metadata[f"{side}_u"])], [initial_p], color=color,
                     s=55, zorder=4)
        axis.annotate("L" if side == "left" else "R",
                      (float(metadata[f"{side}_u"]), initial_p),
                      xytext=(5, 5), textcoords="offset points")

    u_star = float(metadata["u_star"])
    p_star = float(metadata["p_star"])
    axis.scatter([u_star], [p_star], color="black", marker="*", s=150,
                 zorder=5, label="(u*, p*)")
    axis.annotate("*", (u_star, p_star), xytext=(7, 6), textcoords="offset points")
    if number >= 3:
        axis.set_yscale("log")
    axis.set(xlabel="Velocity u", ylabel="Pressure p",
             title=f"Toro test {number}: wave curves in the p-u plane")
    axis.grid(alpha=0.2, which="both")
    axis.legend(loc="best", fontsize=9)
    figure.tight_layout()
    figure_dir.mkdir(parents=True, exist_ok=True)
    figure.savefig(figure_dir / f"toro_{number}_pu.png", dpi=180)
    plt.close(figure)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--data-dir", type=Path, default=Path("output/cases"))
    parser.add_argument("--figure-dir", type=Path, default=Path("docs/figures"))
    args = parser.parse_args()
    for number in range(1, 6):
        plot_case(args.data_dir / f"toro_{number}", args.figure_dir, number)
        print(f"Saved Toro {number} p-u diagram")


if __name__ == "__main__":
    main()
