#!/bin/python3

# -*- coding: utf-8 -*-

import sys
import random

# Pick a random quote and color

quotes = [
("“Simplicity is the soul of efficiency.” — Austin Freeman", "#6eff9a"),
("“Code is like humor. When you have to explain it, it’s bad.” — Cory House", "#f5e960"),
("“Programs must be written for people to read.” — Harold Abelson", "#60e0f5"),
("“Experience is the name everyone gives to their mistakes.” — Oscar Wilde", "#ff5733"),
]
quote, color = random.choice(quotes)

# Strict RFC headers with CRLF

sys.stdout.write("Status: 200 OK\r\n")
sys.stdout.write("Content-Type: text/html; charset=utf-8\r\n")
sys.stdout.write("\r\n")  # Exactly one CRLF CRLF separator

# HTML body

sys.stdout.write(f"""<!DOCTYPE html>

<html lang="en">
<head>
<meta charset="UTF-8">
<title>RFC CGI Demo</title>
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
  font-size: 2.2em;
  text-shadow: 0 0 10px {color}, 0 0 20px {color};
}}
p {{
  font-size: 1em;
  color: #bbb;
}}
</style>
</head>
<body>
  <h1>💡 RFC-Compliant CGI Output</h1>
  <p>{quote}</p>
</body>
</html>
""")
