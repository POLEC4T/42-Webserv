#!/usr/bin/env php
<?php
// -*- coding: utf-8 -*-
// chmod +x clock.php
// Executes fine under /bin/php in a CGI context

date_default_timezone_set('Europe/Paris');

// Send CGI headers with explicit CRLF
echo "Status: 200 OK\r\n";
echo "Content-Type: text/html; charset=utf-8\r\n";
echo "\r\n"; // Empty line separating headers and body

// Get current time & date
$time = date("H:i:s");
$date = date("l, d F Y");

// Simple ASCII "matrix-like" animation background
$matrix = "";
for ($i = 0; $i < 1000; $i++) {
    $matrix .= rand(0, 1) ? "0" : "1";
    if ($i % 80 == 79) $matrix .= "\n";
}

// HTML + CSS output
echo "<!DOCTYPE html>
<html lang='en'>
<head>
<meta charset='UTF-8'>
<title>Retro Terminal Clock</title>
<style>
body {
  background-color: black;
  color: #33ff33;
  font-family: 'Courier New', monospace;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  height: 100vh;
  margin: 0;
  overflow: hidden;
}
.clock {
  font-size: 4em;
  text-shadow: 0 0 10px #33ff33;
  animation: blink 1s infinite alternate;
}
.date {
  color: #00ff99;
  font-size: 1.3em;
  margin-top: 10px;
}
.matrix {
  position: fixed;
  inset: 0;
  color: rgba(0,255,0,0.1);
  white-space: pre;
  overflow: hidden;
  z-index: -1;
  font-size: 10px;
}
@keyframes blink {
  from { opacity: 1; }
  to { opacity: 0.7; }
}
</style>
<meta http-equiv='refresh' content='1'>
</head>
<body>
<div class='matrix'>".htmlspecialchars($matrix)."</div>
<div class='clock'>{$time}</div>
<div class='date'>{$date}</div>
</body>
</html>";
?>
