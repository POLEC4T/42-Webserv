#!/usr/bin/php
<?php declare(strict_types=1);

// ---- CGI response ----
echo "HTTP/1.1 200 OK\r\n";
echo "Content-Type: text/html; charset=UTF-8\r\n";
echo "\r\n"; // Ligne vide obligatoire entre headers et body

// ---- Body HTML ----
echo "<!DOCTYPE html>
<html lang='fr'>
<head>
<meta charset='UTF-8'>
<title>Fractal CGI</title>
<style>
body{font-family:'Noto Sans Mono',monospace;margin:0;padding:10px;background:#111;color:#ddd;}
table{border-collapse:collapse;}
td{width:2px;height:2px;padding:0;margin:0;}
</style>
</head>
<body>
<table>\n";

for($y=0;$y<$heigth;$y++){
    echo "<tr>";
    for($x=0;$x<$width;$x++){
        $z=['real'=>0.285,'imag'=>0.01];
        $c=[
            'real'=>($x-($width/2))/$heigth/$camera_zoom+$camera_pos['x'],
            'imag'=>($y-($heigth/2))/$heigth/$camera_zoom+$camera_pos['y']
        ];
        $iter = $fractalCompute($z,$c,$max_iter);
        $r = iterToRed($iter,$max_iter);
        $g = iterToGreen($iter,$max_iter);
        $b = iterToBlue($iter,$max_iter);
        echo displayCell($r,$g,$b);
    }
    echo "</tr>\n";
}

echo "</table>
</body>
</html>";

$body = ob_get_clean();

// -------------------- Envoi des headers CGI --------------------
$length = strlen($body);
echo "HTTP/1.1 200 OK\r\n";
echo "Content-Type: text/html; charset=UTF-8\r\n";
echo "Content-Length: $length\r\n";
echo "\r\n";

// -------------------- Envoi du body --------------------
echo $body;

?>
