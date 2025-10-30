#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import cgitb
cgitb.enable()

# --- Génération du contenu HTML ---
html_body = """<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Snake (CGI)</title>
    <style>
        body { margin-top: 100px; background: #222; color: white; text-align: center; font-family: sans-serif; }
        canvas { background: #111; margin-top: 20px; border: 2px solid #444; }
    </style>
</head>
<body>
    <h1>🐍 Snake en Python (CGI)</h1>
    <canvas id="game" width="600" height="400"></canvas>
    <p> Arrow to change Direction </p>

    <script>
        const canvas = document.getElementById('game');
        const ctx = canvas.getContext('2d');

        const box = 20;
        let snake = [{x: 9 * box, y: 10 * box}];
        let direction = "RIGHT";
        let food = {
            x: Math.floor(Math.random() * 19 + 1) * box,
            y: Math.floor(Math.random() * 19 + 1) * box
        };
        let score = 0;

        document.addEventListener("keydown", directionChange);

        function directionChange(event) {
            let key = event.keyCode;
            if (key === 37 && direction !== "RIGHT") direction = "LEFT";
            else if (key === 38 && direction !== "DOWN") direction = "UP";
            else if (key === 39 && direction !== "LEFT") direction = "RIGHT";
            else if (key === 40 && direction !== "UP") direction = "DOWN";
        }

        function collision(head, array) {
            for (let i = 0; i < array.length; i++) {
                if (head.x === array[i].x && head.y === array[i].y) return true;
            }
            return false;
        }

        function draw() {
            ctx.fillStyle = "#111";
            ctx.fillRect(0, 0, 600, 400);

            for (let i = 0; i < snake.length; i++) {
                ctx.fillStyle = i === 0 ? "#0f0" : "#0a0";
                ctx.fillRect(snake[i].x, snake[i].y, box, box);
            }

            ctx.fillStyle = "#f00";
            ctx.fillRect(food.x, food.y, box, box);

            let snakeX = snake[0].x;
            let snakeY = snake[0].y;

            if (direction === "LEFT") snakeX -= box;
            if (direction === "UP") snakeY -= box;
            if (direction === "RIGHT") snakeX += box;
            if (direction === "DOWN") snakeY += box;

            if (snakeX === food.x && snakeY === food.y) {
                score++;
                food = {
                    x: Math.floor(Math.random() * 19 + 1) * box,
                    y: Math.floor(Math.random() * 19 + 1) * box
                };
            } else {
                snake.pop();
            }

            let newHead = {x: snakeX, y: snakeY};

            if (snakeX < -20 || snakeY < -20 || snakeX > 600 || snakeY > 400 || collision(newHead, snake)) {
                clearInterval(game);
                alert("Game Over ! Score : " + score);
            }

            snake.unshift(newHead);

            ctx.fillStyle = "white";
            ctx.font = "20px Arial";
            ctx.fillText("Score : " + score, 10, 20);
        }

        let game = setInterval(draw, 150);
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
