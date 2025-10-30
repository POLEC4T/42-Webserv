#!/usr/bin/php
<?php declare(strict_types=1);

// -------------------- Fonctions fractales --------------------
function squaredNorm(array $c): float {
    return $c['real']*$c['real'] + $c['imag']*$c['imag'];
}

function absComplex(array $c): array {
    return ['real'=>abs($c['real']),'imag'=>abs($c['imag'])];
}

function addComplex(array $a, array $b): array {
    return ['real'=>$a['real']+$b['real'],'imag'=>$a['imag']+$b['imag']];
}

function squareComplex(array $c): array {
    return ['real'=>$c['real']*$c['real']-$c['imag']*$c['imag'],'imag'=>2*$c['real']*$c['imag']];
}

function addComplexSquared(array $a, array $b): array {
    return addComplex(squareComplex($a),$b);
}

function mandelbrotComputeIteration(array $z,array $c,int $max_iter): int {
    $i=0;
    while($i<$max_iter && squaredNorm($z)<=4) {
        $z = addComplexSquared($z,$c);
        $i++;
    }
    return $i;
}

function juliaComputeIteration(array $z,array $c,int $max_iter): int {
    $i=0;
    while($i<$max_iter && squaredNorm($c)<=4) {
        $c = addComplexSquared($c,$z);
        $i++;
    }
    return $i;
}

function burningShipComputeIteration(array $z,array $c,int $max_iter): int {
    $i=0;
    while($i<$max_iter && squaredNorm($z)<=4) {
        $z = addComplexSquared(absComplex($z),$c);
        $i++;
    }
    return $i;
}

// -------------------- Fonctions couleurs --------------------
function iterToRed(int $iter,int $max_iter): int {
    $t = $iter/$max_iter;
    return (int)(9*(1-$t)*$t*$t*$t*255);
}

function iterToGreen(int $iter,int $max_iter): int {
    $t = $iter/$max_iter;
    return (int)(15*(1-$t)*(1-$t)*$t*$t*255);
}

function iterToBlue(int $iter,int $max_iter): int {
    $t = $iter/$max_iter;
    return (int)(8.5*(1-$t)*(1-$t)*(1-$t)*$t*255);
}

function displayCell(int $r,int $g,int $b): string {
    return "<td style='background-color: rgb($r,$g,$b);'></td>";
}

// -------------------- Lecture des paramètres CGI (GET ou POST) --------------------
function parseParams(string $data): array {
    $params = [];
    parse_str($data, $params);
    return $params;
}

// Récupération de la méthode
$method = $_SERVER['REQUEST_METHOD'] ?? 'GET';

$params = [];
if ($method === 'POST') {
    // Lire le body depuis STDIN selon Content-Length
    $length = (int)($_SERVER['CONTENT_LENGTH'] ?? 0);
    if ($length > 0) {
        $postData = fread(STDIN, $length);
        $params = parseParams($postData);
    }
} else {
    // GET : récupérer depuis QUERY_STRING
    $queryString = $_SERVER['QUERY_STRING'] ?? '';
    $params = parseParams($queryString);
}

// Valeurs par défaut si vide
$heigth = (int)($params['heigth'] ?? 100);
$width  = (int)($params['width'] ?? 200);
$max_iter = (int)($params['iteration'] ?? 50);

$fractalMap = [
    'mandelbrot'    => 'mandelbrotComputeIteration',
    'julia'         => 'juliaComputeIteration',
    'burningShip'   => 'burningShipComputeIteration'
];
$fractalParam = $params['fractal'] ?? 'mandelbrot';
$fractalCompute = $fractalMap[$fractalParam] ?? 'mandelbrotComputeIteration';

$camera_pos = ['x'=>-0.5, 'y'=>0.0];
$camera_pos['x'] = (float)($params['cam_x'] ?? $camera_pos['x']);
$camera_pos['y'] = (float)($params['cam_y'] ?? $camera_pos['y']);
$camera_zoom = (float)($params['zoom'] ?? 0.75);

// -------------------- Génération du body HTML --------------------
ob_start();

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
