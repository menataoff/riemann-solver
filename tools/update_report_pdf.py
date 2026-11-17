"""Обновляет разделы 4–6 отчёта из docs/report_sections_4_6.md."""

from pathlib import Path
import tempfile

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
from matplotlib.font_manager import FontProperties, findfont
from PIL import ImageFont
from pypdf import PdfReader, PdfWriter


ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "docs/report_sections_4_6.md"
REPORT = ROOT / "docs/Группа_04_prac15_редакция.pdf"
PAGE_WIDTH = 595.28
PAGE_HEIGHT = 841.89
LEFT = 47.0
RIGHT = 47.0
TOP = 64.0
BOTTOM = 787.0


def plain(text: str) -> str:
    return text.replace("**", "").replace("`", "")


def font(size: float, mono: bool = False):
    family = "DejaVu Sans Mono" if mono else "DejaVu Sans"
    path = findfont(FontProperties(family=family))
    return ImageFont.truetype(path, round(size * 4))


def wrap(text: str, size: float, width: float, mono: bool = False) -> list[str]:
    current = ""
    lines = []
    measure = font(size, mono)
    for word in text.split():
        candidate = f"{current} {word}" if current else word
        if measure.getlength(candidate) / 4 <= width or not current:
            current = candidate
        else:
            lines.append(current)
            current = word
    if current:
        lines.append(current)
    return lines


def blocks(markdown: str):
    paragraph = []
    in_code = False
    code_lines = []
    for line in markdown.splitlines():
        if line.startswith("```"):
            if paragraph:
                yield "text", plain(" ".join(paragraph))
                paragraph = []
            if in_code:
                yield "codeblock", code_lines
                code_lines = []
                in_code = False
            else:
                in_code = True
            continue
        if in_code:
            code_lines.append(line)
        elif not line.strip():
            if paragraph:
                yield "text", plain(" ".join(paragraph))
                paragraph = []
        elif line.startswith("# ") or line.startswith("## "):
            if paragraph:
                yield "text", plain(" ".join(paragraph))
                paragraph = []
            yield "heading" if line.startswith("# ") else "subheading", line.lstrip("# ")
        elif line.startswith("- "):
            if paragraph:
                yield "text", plain(" ".join(paragraph))
                paragraph = []
            yield "bullet", plain(line[2:])
        else:
            paragraph.append(line.strip())
    if paragraph:
        yield "text", plain(" ".join(paragraph))
    if code_lines:
        yield "codeblock", code_lines


def draw_header(fig, page_number: int) -> None:
    fig.text(LEFT / PAGE_WIDTH, 1 - 27 / PAGE_HEIGHT,
             "Практика к §15 — точный решатель задачи Римана",
             fontsize=7.5, color="#555555", va="top")
    fig.text(1 - RIGHT / PAGE_WIDTH, 1 - 27 / PAGE_HEIGHT,
             "Группа 04 / Б23-221", fontsize=7.5, color="#555555",
             ha="right", va="top")
    fig.text(0.5, 1 - 817 / PAGE_HEIGHT, str(page_number),
             fontsize=8, ha="center", va="top")


def create_pages(source: str, pdf_path: Path, preview_dir: Path) -> int:
    plt.rcParams["pdf.fonttype"] = 42
    preview_dir.mkdir(parents=True, exist_ok=True)
    page_number = 8
    fig = None
    y = TOP

    def new_page():
        nonlocal fig, y
        fig = plt.figure(figsize=(PAGE_WIDTH / 72, PAGE_HEIGHT / 72))
        draw_header(fig, page_number)
        y = TOP

    def finish_page(pdf):
        if fig is None:
            return
        pdf.savefig(fig)
        fig.savefig(preview_dir / f"page_{page_number}.png", dpi=100)
        plt.close(fig)

    def draw(text: str, size: float, height: float, x: float = LEFT,
             weight: str = "normal", mono: bool = False):
        nonlocal y, page_number
        if y + height > BOTTOM:
            finish_page(pdf)
            page_number += 1
            new_page()
        fig.text(x / PAGE_WIDTH, 1 - y / PAGE_HEIGHT, text,
                 fontsize=size, fontweight=weight,
                 fontfamily="DejaVu Sans Mono" if mono else "DejaVu Sans",
                 va="top")
        y += height

    def ensure_space(height: float):
        nonlocal y, page_number
        if y + height > BOTTOM:
            finish_page(pdf)
            page_number += 1
            new_page()

    with PdfPages(pdf_path) as pdf:
        new_page()
        for kind, value in blocks(source):
            if kind == "heading":
                ensure_space(95)
                y += 7
                for line in wrap(value, 14, PAGE_WIDTH - LEFT - RIGHT):
                    draw(line, 14, 20, weight="bold")
                y += 7
            elif kind == "subheading":
                ensure_space(95)
                y += 6
                for line in wrap(value, 11.3, PAGE_WIDTH - LEFT - RIGHT):
                    draw(line, 11.3, 17, weight="bold")
                y += 4
            elif kind == "codeblock":
                ensure_space(len(value) * 12 + 6)
                for line in value:
                    draw(line, 8.4, 12, x=LEFT + 10, mono=True)
                y += 6
            else:
                x = LEFT + 12 if kind == "bullet" else LEFT
                width = PAGE_WIDTH - x - RIGHT
                lines = wrap(value, 9.1, width)
                gap = 5 if kind == "bullet" else 7
                ensure_space(len(lines) * 13 + gap)
                for index, line in enumerate(lines):
                    prefix = "• " if kind == "bullet" and index == 0 else ""
                    draw(prefix + line, 9.1, 13, x=x)
                y += gap
        finish_page(pdf)
    return page_number


def main() -> None:
    source = SOURCE.read_text(encoding="utf-8")
    report = PdfReader(str(REPORT))
    if len(report.pages) < 7:
        raise RuntimeError("В исходном отчёте меньше семи страниц")

    with tempfile.TemporaryDirectory(prefix="prac15_report_") as temp:
        directory = Path(temp)
        revised = directory / "revised_sections.pdf"
        last_page = create_pages(source, revised, directory / "preview")
        writer = PdfWriter()
        for page in report.pages[:7]:
            writer.add_page(page)
        for page in PdfReader(str(revised)).pages:
            writer.add_page(page)
        writer.add_metadata({"/Title": "Точный решатель задачи Римана и архитектура комплекса",
                             "/Subject": "Исправленные разделы 4–6"})
        final = directory / "report.pdf"
        with final.open("wb") as file:
            writer.write(file)
        REPORT.write_bytes(final.read_bytes())
        print(f"Обновлены страницы 8–{last_page} в {REPORT}")


if __name__ == "__main__":
    main()
