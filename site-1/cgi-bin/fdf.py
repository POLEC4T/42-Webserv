#!/usr/bin/env python3
import math
import sys
import os

# --- Full HTTP header for your own webserver ---
sys.stdout.write("HTTP/1.1 200 OK\r\n")
sys.stdout.write("Content-Type: text/html\r\n")
sys.stdout.write("\r\n")
sys.stdout.flush()

# ===============================
# Default embedded FDF map
# ===============================
DEFAULT_MAP = """
0 0 0 0 0
0 1 2 1 0
0 2 4 2 0
0 1 2 1 0
0 0 0 0 0
""".strip().splitlines()

def load_default_map():
    return [list(map(int, line.split())) for line in DEFAULT_MAP]

def iso_projection(x, y, z, spacing=20, height=5):
    angle = math.radians(30)
    X = (x - y) * math.cos(angle)
    Y = (x + y) * math.sin(angle) - z * height / 10.0
    return X * spacing, Y * spacing

def generate_svg(grid):
    h, w = len(grid), len(grid[0])
    svg_lines = []
    for y in range(h):
        for x in range(w):
            x1, y1 = iso_projection(x, y, grid[y][x])
            if x < w - 1:
                x2, y2 = iso_projection(x + 1, y, grid[y][x + 1])
                svg_lines.append(f'<line x1="{x1}" y1="{y1}" x2="{x2}" y2="{y2}" stroke="white" stroke-width="1"/>')
            if y < h - 1:
                x3, y3 = iso_projection(x, y + 1, grid[y + 1][x])
                svg_lines.append(f'<line x1="{x1}" y1="{y1}" x2="{x3}" y2="{y3}" stroke="white" stroke-width="1"/>')
    return "\n".join(svg_lines)

grid = load_default_map()
svg_content = generate_svg(grid)

html = f"""<!DOCTYPE html>
<html>
<head>
<title>FDF CGI Wireframe</title>
<style>
body {{
  background-color: #101020;
  color: white;
  text-align: center;
  font-family: monospace;
}}
svg {{
  background-color: #202040;
  margin-top: 40px;
  transform: scale(1.5) translate(150px, 100px);
}}
</style>
</head>
<body>
<h2>FDF Python CGI 🌐</h2>
<svg width="600" height="400">
{svg_content}
</svg>
</body>
</html>
"""

sys.stdout.write(html)
sys.stdout.flush()
