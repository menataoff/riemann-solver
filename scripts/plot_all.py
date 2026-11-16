"""Build all figures for the five Toro cases from exported CSV files."""

from pathlib import Path
import argparse

from plot_profiles import plot_case as plot_profiles
from plot_xt import plot_case as plot_xt
from plot_pu import plot_case as plot_pu


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--data-dir", type=Path, default=Path("output/cases"))
    parser.add_argument("--figure-dir", type=Path, default=Path("docs/figures"))
    args = parser.parse_args()
    for number in range(1, 6):
        case_dir = args.data_dir / f"toro_{number}"
        plot_profiles(case_dir, args.figure_dir, number)
        plot_xt(case_dir, args.figure_dir, number)
        plot_pu(case_dir, args.figure_dir, number)
        print(f"Saved all figures for Toro {number}")


if __name__ == "__main__":
    main()
