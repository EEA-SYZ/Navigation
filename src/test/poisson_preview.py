#!/usr/bin/env python3
"""Preview raw planar graphs for the navigation data generator."""

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
    address: tuple[int, ...] = ()
    level: int = 0


@dataclass(frozen=True)
class Edge:
    name: str
    from_name: str
    to_name: str
    length: float = 0.0
    volume: int = 0
    level: int = 0


@dataclass(frozen=True)
class GraphData:
    points: list[Point]
    edges: list[Edge]


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
        radius = math.sqrt(width * height * 0.45 / target_count)

    rng = random.Random(seed)
    nominal_radius = max(radius, 1e-9)
    relax = 1.0
    min_radius = nominal_radius * 0.72
    max_radius = nominal_radius * 1.08
    cell_size = min_radius / math.sqrt(2.0)
    cols = max(1, math.ceil(width / cell_size))
    rows = max(1, math.ceil(height / cell_size))
    grid: list[list[int]] = [[] for _ in range(cols * rows)]
    points: list[Point] = []
    point_radii: list[float] = []

    def grid_xy(point: Point) -> tuple[int, int]:
        gx = min(cols - 1, max(0, int((point.x - left) / cell_size)))
        gy = min(rows - 1, max(0, int((point.y - bottom) / cell_size)))
        return gx, gy

    def add_point(point: Point, point_radius: float) -> None:
        gx, gy = grid_xy(point)
        grid[gy * cols + gx].append(len(points))
        points.append(point)
        point_radii.append(point_radius)

    def valid(x: float, y: float, candidate_radius: float, query_radius: float) -> bool:
        if x < left or x > right or y < bottom or y > top:
            return False

        gx = min(cols - 1, max(0, int((x - left) / cell_size)))
        gy = min(rows - 1, max(0, int((y - bottom) / cell_size)))
        cell_range = max(1, math.ceil(query_radius / cell_size))
        for yy in range(max(0, gy - cell_range), min(rows, gy + cell_range + 1)):
            for xx in range(max(0, gx - cell_range), min(cols, gx + cell_range + 1)):
                for idx in grid[yy * cols + xx]:
                    other = points[idx]
                    dx = x - other.x
                    dy = y - other.y
                    local_radius = min(candidate_radius, point_radii[idx])
                    if dx * dx + dy * dy < local_radius * local_radius:
                        return False
        return True

    failed_attempts = 0
    max_failed_attempts = max(2000, target_count * max(80, attempts * 4))
    while len(points) < target_count:
        x = rng.uniform(left, right)
        y = rng.uniform(bottom, top)
        candidate_radius = nominal_radius * relax * rng.uniform(0.82, 1.12)
        boundary_distance = min(x - left, right - x, y - bottom, top - y)
        boundary_blend = max(0.0, min(1.0, boundary_distance / nominal_radius))
        candidate_radius *= 0.80 + 0.20 * boundary_blend
        candidate_radius = max(
            min_radius * relax,
            min(max_radius * relax, candidate_radius),
        )

        if valid(x, y, candidate_radius, max_radius * relax):
            add_point(Point(f"N{len(points)}", x, y), candidate_radius)
            failed_attempts = 0
            continue

        failed_attempts += 1
        if failed_attempts >= max_failed_attempts:
            relax *= 0.94
            min_radius *= 0.94
            max_radius *= 0.94
            failed_attempts = 0

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


def write_csv(path: Path, graph: GraphData) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.writer(handle)
        writer.writerow(
            ["kind", "name", "x", "y", "address", "level", "from", "to", "length", "volume", "edge_level"]
        )
        for point in graph.points:
            writer.writerow(
                [
                    "node",
                    point.name,
                    point.x,
                    point.y,
                    ";".join(str(item) for item in point.address),
                    point.level,
                    "",
                    "",
                    "",
                    "",
                    "",
                ]
            )
        for edge in graph.edges:
            writer.writerow(
                [
                    "edge",
                    edge.name,
                    "",
                    "",
                    "",
                    "",
                    edge.from_name,
                    edge.to_name,
                    edge.length,
                    edge.volume,
                    edge.level,
                ]
            )


def parse_address(value: str | None) -> tuple[int, ...]:
    if not value:
        return ()
    return tuple(int(item) for item in value.replace("|", ";").split(";") if item)


def infer_level(address: tuple[int, ...]) -> int:
    if not address:
        return 0
    self_id = address[0]
    level = 0
    for idx, parent_id in enumerate(address[1:], start=1):
        if parent_id == self_id:
            level = idx
    return level


def read_csv_rows(handle: TextIO) -> GraphData:
    points: list[Point] = []
    edges: list[Edge] = []
    reader = csv.DictReader(handle)
    for idx, row in enumerate(reader):
        kind = (row.get("kind") or "node").strip().lower()
        if kind == "edge":
            edge_level = row.get("edge_level") or row.get("level") or "0"
            volume = row.get("volume") or "0"
            length = row.get("length") or "0"
            edges.append(
                Edge(
                    row.get("name") or f"E{len(edges)}",
                    row.get("from") or "",
                    row.get("to") or "",
                    float(length),
                    int(volume),
                    int(edge_level),
                )
            )
            continue

        name = row.get("name") or f"N{idx}"
        address = parse_address(row.get("address"))
        level_text = row.get("level")
        level = int(level_text) if level_text not in (None, "") else infer_level(address)
        points.append(Point(name, float(row["x"]), float(row["y"]), address, level))
    return GraphData(points, edges)


def read_csv(path: Path) -> GraphData:
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
    edges: list[Edge],
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
    from matplotlib.collections import LineCollection

    fig, ax = plt.subplots(figsize=(30, 30))
    palette = [
        "#2563eb",
        "#dc2626",
        "#16a34a",
        "#9333ea",
        "#ea580c",
        "#0891b2",
        "#be123c",
        "#4d7c0f",
        "#7c3aed",
        "#0f766e",
    ]
    edge_palette = [
        "#94a3b8",
        "#fca5a5",
        "#86efac",
        "#c4b5fd",
        "#fdba74",
        "#67e8f9",
        "#fda4af",
        "#bef264",
        "#ddd6fe",
        "#99f6e4",
    ]
    levels = sorted({point.level for point in points})
    has_hierarchy = len(levels) > 1 or any(point.level > 0 or point.address for point in points)
    points_by_name = {point.name: point for point in points}

    if edges:
        for level in sorted({edge.level for edge in edges}):
            segments = []
            for edge in edges:
                if edge.level != level:
                    continue
                start = points_by_name.get(edge.from_name)
                end = points_by_name.get(edge.to_name)
                if start is None or end is None:
                    continue
                segments.append([(start.x, start.y), (end.x, end.y)])
            if not segments:
                continue
            ax.add_collection(
                LineCollection(
                    segments,
                    colors=edge_palette[level % len(edge_palette)],
                    linewidths=0.60 + 0.25 * min(level, 4),
                    alpha=0.20 + 0.20 * min(level, 3),
                    zorder=1,
                )
            )

    if has_hierarchy:
        for level in levels:
            layer = [point for point in points if point.level == level]
            xs = [point.x for point in layer]
            ys = [point.y for point in layer]

            size = 16 + 4 ** level + 30 * level
            
            ax.scatter(
                xs,
                ys,
                s=size,
                color=palette[level % len(palette)],
                edgecolors="#0f172a",
                linewidths=0.35,
                label=f"nodes L{level}",
                alpha=0.88,
                zorder=3 + level,
            )
        ax.legend(loc="upper right", frameon=True)
    else:
        xs = [point.x for point in points]
        ys = [point.y for point in points]
        ax.scatter(
            xs,
            ys,
            s=18,
            color="#2563eb",
            edgecolors="#0f172a",
            linewidths=0.35,
            zorder=3,
        )

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
        description="Generate and preview a raw planar navigation graph."
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
        help="plot an existing graph CSV with node and edge rows",
    )
    parser.add_argument("--stdin", action="store_true", help="read graph CSV from stdin")
    parser.add_argument("--save", type=Path, default=None, help="save preview image")
    parser.add_argument("--csv", type=Path, default=None, help="write generated graph CSV")
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
        graph = read_csv_rows(sys.stdin)
        radius = args.radius or 0.0
    elif args.input_csv:
        graph = read_csv(args.input_csv)
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
        graph = GraphData(points, [])
    points = graph.points
    edges = graph.edges
    closest = nearest_distance(points)
    if args.stdin:
        print(f"loaded from stdin: {len(points)} nodes, {len(edges)} edges")
    elif args.input_csv:
        print(f"loaded: {len(points)} nodes, {len(edges)} edges")
    else:
        print(f"generated: {len(points)} / {args.count}")
        print(f"radius: {radius:.4f}")
    if closest is not None:
        print(f"nearest distance: {closest:.4f}")

    if args.csv:
        write_csv(args.csv, graph)
        print(f"csv: {args.csv}")

    level_count = 0
    if points:
        level_count = max((len(point.address) for point in points), default=0)
        if level_count == 0:
            level_count = max((point.level for point in points), default=0) + 1
    if edges:
        title = f"Raw planar graph: {len(points)} nodes, {len(edges)} edges, {level_count} levels"
    else:
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
        edges,
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
