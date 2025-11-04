#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import random

# Predefined moods
moods = [
    ("🌞", "Bright and optimistic", "#f5e960"),
    ("🌧️", "Calm and reflective", "#6ec1e4"),
    ("🔥", "Bold and energetic", "#ff5733"),
    ("🌿", "Peaceful and grounded", "#6eff9a"),
    ("🌌", "Mysterious and curious", "#9f6eff"),
    ("🌊", "Chill and adaptive", "#60e0f5"),
    ("🌈", "Creative and joyful", "#ff9fee"),
]

emoji, mood, color = random.choice(moods)

# Manually write RFC-compliant headers
sys.stdout.write("Status: 200 OK\r\n")
sys.stdout.write("Content-Type: text/html; charset=utf-8\r\n")
sys.stdout.write("\r\n")  # Explicit \r\n\r\n between header and body

# Body
html = f"""<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>Your Mood Generator</title>
<style>
body {{
  background: radial-gradient(circle at center, {color}33, #101010);
  color: white;
  font-family: 'Fira Sans', sans-serif;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  height: 100vh;
  margin: 0;
}}
h1 {{
  font-size: 3em;
  text-shadow: 0 0 20px {color};
  animation: pulse 2s infinite alternate;
}}
p {{
  color: #ccc;
  font-size: 1.2em;
}}
@keyframes pulse {{
  from {{ text-shadow: 0 0 10px {color}; }}
  to {{ text-shadow: 0 0 30px {color}; }}
}}
</style>
</head>
<body>
  <h1>{emoji} Your mood is: {mood}</h1>
  <p>Refreshed at every reload ✨</p>
</body>
</html>
"""

sys.stdout.write(html)
sys.stdout.flush()
