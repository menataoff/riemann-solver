"""Строит три рисунка для одной задачи по четырём CSV от export_case."""

from pathlib import Path
import argparse
import re

from plot_profiles import plot_case as plot_profiles
from plot_xt import plot_case as plot_xt
from plot_pu import plot_case as plot_pu


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("data_dir", type=Path, help="Каталог с четырьмя CSV-файлами")
    parser.add_argument("--figure-dir", type=Path, default=Path("output/figures"))
    parser.add_argument("--name", help="Короткое имя для заголовков и имён файлов")
    parser.add_argument("--pressure-scale", choices=("auto", "linear", "log"), default="auto")
    args = parser.parse_args()

    name = args.name or args.data_dir.name
    if not re.fullmatch(r"[A-Za-z0-9_-]+", name):
        parser.error("--name может содержать только латинские буквы, цифры, '_' и '-'")
    log_pressure = None if args.pressure_scale == "auto" else args.pressure_scale == "log"

    plot_profiles(args.data_dir, args.figure_dir, name)
    plot_xt(args.data_dir, args.figure_dir, name)
    plot_pu(args.data_dir, args.figure_dir, name, log_pressure)
    print(f"В {args.figure_dir} сохранены {name}_profiles.png, "
          f"{name}_xt.png и {name}_pu.png")


if __name__ == "__main__":
    main()
