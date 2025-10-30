#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import cgitb
cgitb.enable()

# --- Génération du contenu HTML ---
html_body = """<!DOCTYPE html>
<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <title>PixelVerse — Snake (CGI)</title>

  <script src="https://cdn.tailwindcss.com"></script>
  <link href="https://fonts.googleapis.com/css2?family=Share+Tech+Mono&display=swap" rel="stylesheet">

  <style>
    body {
      font-family: 'Share Tech Mono', monospace;
      background-color: #0a0a0a;
      color: #d4d4d4;
      letter-spacing: 0.4px;
      text-align: center;
    }

    header {
      background: rgba(15, 15, 15, 0.8);
      backdrop-filter: blur(8px);
      border-bottom: 1px solid #2a2a2a;
    }

    canvas {
      background: #111;
      border: 2px solid #2a2a2a;
      border-radius: 6px;
      margin: auto;
      margin-top: 5rem;
      box-shadow: 0 0 15px rgba(0, 255, 0, 0.05);
    }

    .info {
      color: #888;
      font-size: 0.9rem;
      margin-top: 1.2rem;
    }

    .section {
      max-width: 800px;
      margin: 8rem auto 4rem;
      padding: 2rem;
    }
  </style>
</head>

<body>
  <!-- HEADER -->
  <header class="fixed top-0 left-0 w-full p-5 text-center z-50">
    <h1 class="text-xl text-gray-200 tracking-wider">PIXELVERSE</h1>
    <nav class="mt-3 space-x-8 text-sm text-gray-500">
      <a href="../index.html">Accueil</a>
      <a href="../requete.html">Testing</a>
      <a href="../index.html#games" class="text-white">Jeux</a>
      <a href="../index.html#profile" class="text-white">Profil</a>
    </nav>
  </header>

  <!-- CONTENU -->
  <section class="section">
    <h2 class="text-2xl text-gray-100 mb-4">🐍 Snake s</h2>
    <canvas id="game" width="600" height="400"></canvas>
    <p class="info">Utilisez les flèches du clavier pour contrôler le serpent.</p>
  </section>

  <!-- SCRIPT DU JEU -->
  <script>
    const canvas = document.getElementById('game');
    const ctx = canvas.getContext('2d');
    const box = 20;

    let snake = [{ x: 9 * box, y: 10 * box }];
    let direction = "RIGHT";
    let food = {
      x: Math.floor(Math.random() * 29) * box,
      y: Math.floor(Math.random() * 19) * box
    };
    let score = 0;

    document.addEventListener("keydown", e => {
      if (e.key === "ArrowLeft" && direction !== "RIGHT") direction = "LEFT";
      else if (e.key === "ArrowUp" && direction !== "DOWN") direction = "UP";
      else if (e.key === "ArrowRight" && direction !== "LEFT") direction = "RIGHT";
      else if (e.key === "ArrowDown" && direction !== "UP") direction = "DOWN";
    });

    function collision(head, array) {
      return array.some(part => head.x === part.x && head.y === part.y);
    }

    function draw() {
      ctx.fillStyle = "#111";
      ctx.fillRect(0, 0, 600, 400);

      snake.forEach((part, i) => {
        ctx.fillStyle = i === 0 ? "#0f0" : "#0a0";
        ctx.fillRect(part.x, part.y, box, box);
      });

      ctx.fillStyle = "#f00";
      ctx.fillRect(food.x, food.y, box, box);

      let headX = snake[0].x;
      let headY = snake[0].y;

      if (direction === "LEFT") headX -= box;
      if (direction === "UP") headY -= box;
      if (direction === "RIGHT") headX += box;
      if (direction === "DOWN") headY += box;

      if (headX === food.x && headY === food.y) {
        score++;
        food = {
          x: Math.floor(Math.random() * 29) * box,
          y: Math.floor(Math.random() * 19) * box
        };
      } else {
        snake.pop();
      }

      const newHead = { x: headX, y: headY };

      if (
        headX < 0 || headY < 0 ||
        headX >= 600 || headY >= 400 ||
        collision(newHead, snake)
      ) {
        clearInterval(game);
        ctx.fillStyle = "#f33";
        ctx.font = "30px 'Share Tech Mono'";
        ctx.fillText("GAME OVER — Score: " + score, 100, 200);
        return;
      }

      snake.unshift(newHead);

      ctx.fillStyle = "#bbb";
      ctx.font = "16px 'Share Tech Mono'";
      ctx.fillText("Score : " + score, 10, 20);
    }

    const game = setInterval(draw, 120);
  </script>
</body>
</html>"""


# --- Création des en-têtes HTTP complets ---
body_bytes = html_body.encode("utf-8")
content_length = len(body_bytes)

response_headers = f"""HTTP/1.1 200 OK
Content-Type: text/html; charset=utf-8
Content-Length: {content_length}
Connection: close

""".encode("utf-8")

# --- Envoi de la réponse HTTP complète ---
import sys
sys.stdout.buffer.write(response_headers + body_bytes)
