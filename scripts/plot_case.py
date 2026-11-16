"""Plot one custom Riemann case from four CSV files written by export_case."""

from pathlib import Path
import argparse
import re

from plot_profiles import plot_case as plot_profiles
from plot_xt import plot_case as plot_xt
from plot_pu import plot_case as plot_pu


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("data_dir", type=Path, help="Directory containing the four CSV files")
    parser.add_argument("--figure-dir", type=Path, default=Path("output/figures"))
    parser.add_argument("--name", help="Short name used in figure titles and file names")
    parser.add_argument("--pressure-scale", choices=("auto", "linear", "log"), default="auto")
    args = parser.parse_args()

    name = args.name or args.data_dir.name
    if not re.fullmatch(r"[A-Za-z0-9_-]+", name):
        parser.error("--name must contain only letters, digits, '_' or '-'")
    log_pressure = None if args.pressure_scale == "auto" else args.pressure_scale == "log"

    plot_profiles(args.data_dir, args.figure_dir, name)
    plot_xt(args.data_dir, args.figure_dir, name)
    plot_pu(args.data_dir, args.figure_dir, name, log_pressure)
    print(f"Saved {name}_profiles.png, {name}_xt.png and {name}_pu.png to {args.figure_dir}")


if __name__ == "__main__":
    main()
