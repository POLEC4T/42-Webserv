#!/usr/bin/php
<?php
declare(strict_types=1);

// ---- CGI response ----
echo "HTTP/1.1 200 OK\r\n";
echo "Content-Type: text/html; charset=UTF-8\r\n";
echo "\r\n"; // Ligne vide obligatoire entre headers et body

// ---- Body HTML ----
echo "<!DOCTYPE html>
<html lang='fr'>
<head>
    <meta charset='UTF-8'>
    <title>Test PHP CGI</title>
    <style>
        body { font-family: 'Noto Sans Mono', monospace; background: #111; color: #ddd; margin: 0; padding: 10px; }
        table { border-collapse: collapse; }
        td { width: 2px; height: 2px; padding: 0; margin: 0; }
    </style>
</head>
<body>
<table>\n";

// ---- Exemple de contenu ----
$width = 100;
$height = 50;

for ($y = 0; $y < $height; $y++) {
    echo "<tr>";
    for ($x = 0; $x < $width; $x++) {
        $r = ($x/$width)*255;
        $g = ($y/$height)*255;
        $b = 128;
        echo "<td style='background-color: rgb($r,$g,$b);'></td>";
    }
    echo "</tr>\n";
}

echo "</table>
</body>
</html>";
?>
