#!/usr/bin/env python3
"""Preview Poisson disk samples for the navigation data generator."""

from __future__ import annotations

import argparse
import csv
import math
import random
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, TextIO


@dataclass(frozen=True)
class Point:
    name: str
    x: float
    y: float


def poisson_disk_sample(
    left: float,
    right: float,
    bottom: float,
    top: float,
    target_count: int,
    radius: float | None = None,
    attempts: int = 30,
    seed: int | None = None,
) -> tuple[list[Point], float]:
    if target_count <= 0:
        return [], 0.0

    if left > right:
        left, right = right, left
    if bottom > top:
        bottom, top = top, bottom

    width = right - left
    height = top - bottom
    if width <= 0 or height <= 0:
        raise ValueError("bounds must enclose a positive area")

    if radius is None or radius <= 0:
        radius = math.sqrt(width * height * 0.7 / target_count)

    rng = random.Random(seed)
    cell_size = radius / math.sqrt(2.0)
    cols = max(1, math.ceil(width / cell_size))
    rows = max(1, math.ceil(height / cell_size))
    grid: list[int | None] = [None] * (cols * rows)
    points: list[Point] = []
    active: list[int] = []

    def grid_xy(point: Point) -> tuple[int, int]:
        gx = min(cols - 1, max(0, int((point.x - left) / cell_size)))
        gy = min(rows - 1, max(0, int((point.y - bottom) / cell_size)))
        return gx, gy

    def add_point(point: Point) -> None:
        gx, gy = grid_xy(point)
        grid[gy * cols + gx] = len(points)
        active.append(len(points))
        points.append(point)

    def valid(x: float, y: float) -> bool:
        if x < left or x > right or y < bottom or y > top:
            return False

        gx = min(cols - 1, max(0, int((x - left) / cell_size)))
        gy = min(rows - 1, max(0, int((y - bottom) / cell_size)))
        radius_sq = radius * radius
        for yy in range(max(0, gy - 2), min(rows, gy + 3)):
            for xx in range(max(0, gx - 2), min(cols, gx + 3)):
                idx = grid[yy * cols + xx]
                if idx is None:
                    continue
                other = points[idx]
                dx = x - other.x
                dy = y - other.y
                if dx * dx + dy * dy < radius_sq:
                    return False
        return True

    add_point(Point("N0", rng.uniform(left, right), rng.uniform(bottom, top)))

    while active and len(points) < target_count:
        parent_idx = rng.choice(active)
        parent = points[parent_idx]
        found = False

        for _ in range(attempts):
            distance = rng.uniform(radius, radius * 2.0)
            angle = rng.uniform(0.0, math.tau)
            x = parent.x + distance * math.cos(angle)
            y = parent.y + distance * math.sin(angle)

            if valid(x, y):
                add_point(Point(f"N{len(points)}", x, y))
                found = True
                if len(points) >= target_count:
                    break

        if not found:
            active.remove(parent_idx)

    return points, radius


def nearest_distance(points: Iterable[Point]) -> float | None:
    items = list(points)
    if len(items) < 2:
        return None
    best = math.inf
    for i, p in enumerate(items):
        for q in items[i + 1 :]:
            best = min(best, math.hypot(p.x - q.x, p.y - q.y))
    return best


def write_csv(path: Path, points: list[Point]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.writer(handle)
        writer.writerow(["name", "x", "y"])
        for point in points:
            writer.writerow([point.name, point.x, point.y])


def read_csv_rows(handle: TextIO) -> list[Point]:
    points: list[Point] = []
    reader = csv.DictReader(handle)
    for idx, row in enumerate(reader):
        name = row.get("name") or f"N{idx}"
        points.append(Point(name, float(row["x"]), float(row["y"])))
    return points


def read_csv(path: Path) -> list[Point]:
    with path.open("r", newline="", encoding="utf-8") as handle:
        return read_csv_rows(handle)


def bounds_from_points(points: list[Point]) -> tuple[float, float, float, float]:
    if not points:
        return 0.0, 1.0, 0.0, 1.0

    xs = [point.x for point in points]
    ys = [point.y for point in points]
    left = min(xs)
    right = max(xs)
    bottom = min(ys)
    top = max(ys)
    x_pad = max((right - left) * 0.04, 1.0)
    y_pad = max((top - bottom) * 0.04, 1.0)
    return left - x_pad, right + x_pad, bottom - y_pad, top + y_pad


def plot_points(
    points: list[Point],
    left: float,
    right: float,
    bottom: float,
    top: float,
    radius: float,
    output: Path | None,
    show_radius: bool,
    title: str,
    show_window: bool,
) -> None:
    import matplotlib.pyplot as plt
    from matplotlib.patches import Circle

    fig, ax = plt.subplots(figsize=(8, 8))
    xs = [point.x for point in points]
    ys = [point.y for point in points]

    ax.scatter(xs, ys, s=18, color="#2563eb", edgecolors="#0f172a", linewidths=0.35)
    if show_radius:
        for point in points:
            ax.add_patch(
                Circle(
                    (point.x, point.y),
                    radius,
                    fill=False,
                    linewidth=0.35,
                    alpha=0.14,
                    color="#ef4444",
                )
            )

    ax.set_title(title)
    ax.set_xlim(left, right)
    ax.set_ylim(bottom, top)
    ax.set_aspect("equal", adjustable="box")
    ax.grid(True, color="#cbd5e1", linewidth=0.6, alpha=0.7)
    ax.set_xlabel("x")
    ax.set_ylabel("y")

    fig.tight_layout()
    if output:
        output.parent.mkdir(parents=True, exist_ok=True)
        fig.savefig(output, dpi=180)
    if show_window:
        plt.show()
    else:
        plt.close(fig)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Generate and preview a simple Poisson disk sample."
    )
    parser.add_argument("-n", "--count", type=int, default=200, help="target point count")
    parser.add_argument("--left", type=float, default=0.0)
    parser.add_argument("--right", type=float, default=1000.0)
    parser.add_argument("--bottom", type=float, default=0.0)
    parser.add_argument("--top", type=float, default=1000.0)
    parser.add_argument("-r", "--radius", type=float, default=None, help="minimum distance")
    parser.add_argument("-k", "--attempts", type=int, default=30, help="tries per active point")
    parser.add_argument("--seed", type=int, default=None, help="random seed")
    parser.add_argument(
        "--input-csv",
        type=Path,
        default=None,
        help="plot existing points from a CSV with x and y columns",
    )
    parser.add_argument("--stdin", action="store_true", help="read point CSV from stdin")
    parser.add_argument("--save", type=Path, default=None, help="save preview image")
    parser.add_argument("--csv", type=Path, default=None, help="write generated points as CSV")
    parser.add_argument("--no-show", action="store_true", help="do not open a plot window")
    parser.add_argument(
        "--show-radius",
        action="store_true",
        help="draw minimum-distance circles around points",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    if args.stdin:
        points = read_csv_rows(sys.stdin)
        radius = args.radius or 0.0
    elif args.input_csv:
        points = read_csv(args.input_csv)
        radius = args.radius or 0.0
    else:
        points, radius = poisson_disk_sample(
            args.left,
            args.right,
            args.bottom,
            args.top,
            args.count,
            args.radius,
            args.attempts,
            args.seed,
        )
    closest = nearest_distance(points)
    if args.stdin:
        print(f"loaded from stdin: {len(points)}")
    elif args.input_csv:
        print(f"loaded: {len(points)}")
    else:
        print(f"generated: {len(points)} / {args.count}")
        print(f"radius: {radius:.4f}")
    if closest is not None:
        print(f"nearest distance: {closest:.4f}")

    if args.csv:
        write_csv(args.csv, points)
        print(f"csv: {args.csv}")

    title = f"Poisson disk sample: {len(points)} points, r={radius:.2f}"
    if args.stdin or args.input_csv:
        left, right, bottom, top = bounds_from_points(points)
    else:
        left = min(args.left, args.right)
        right = max(args.left, args.right)
        bottom = min(args.bottom, args.top)
        top = max(args.bottom, args.top)

    plot_points(
        points,
        left,
        right,
        bottom,
        top,
        radius,
        args.save,
        args.show_radius,
        title,
        not args.no_show,
    )


if __name__ == "__main__":
    main()
