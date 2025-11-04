#!/usr/bin/env ruby
# -*- coding: utf-8 -*-
# chmod +x citygen.rb
# Can be executed as a CGI via /bin/ruby

# Strict RFC-compliant headers (\r\n\r\n)
STDOUT.write "Status: 200 OK\r\n"
STDOUT.write "Content-Type: text/html; charset=utf-8\r\n"
STDOUT.write "\r\n"

# Name generator logic
prefixes = ["Neo", "Cyber", "Vanta", "Sky", "Ion", "Nova", "Cloud", "Aero", "Shadow", "Lumina"]
suffixes = ["Tokyo", "Haven", "Spire", "District", "Node", "Pulse", "Sector", "Harbor", "Forge", "Vault"]

city = "#{prefixes.sample}-#{suffixes.sample}"

# Random colors for cyberpunk glow
colors = ["#00ffff", "#ff00ff", "#00ff9d", "#ff0077", "#39ff14", "#ffd700"]
glow = colors.sample

# Output the HTML page
puts <<~HTML
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>#{city} — Cyberpunk City Generator</title>
<style>
body {
  background: radial-gradient(circle at center, #{glow}22, #050505);
  color: #{glow};
  font-family: 'Orbitron', sans-serif;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  height: 100vh;
  margin: 0;
  text-shadow: 0 0 20px #{glow};
}
h1 {
  font-size: 4em;
  letter-spacing: 2px;
  text-transform: uppercase;
}
p {
  color: #888;
  font-size: 1.2em;
}
a {
  color: #{glow};
  text-decoration: none;
}
a:hover {
  text-shadow: 0 0 40px #{glow};
}
</style>
<meta http-equiv="refresh" content="2">
</head>
<body>
  <h1>#{city}</h1>
  <p>Welcome to the future — refreshed every 2s ⚡</p>
  <p><a href="?">Generate Again</a></p>
</body>
</html>
HTML
