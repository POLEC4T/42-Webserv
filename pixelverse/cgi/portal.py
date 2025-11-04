#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
CGI Script conforme à RFC 3875 (Common Gateway Interface)
Le serveur web convertira le Status en réponse HTTP/1.1
"""

import os
import sys
import json
from datetime import datetime

def generate_header(status="200 OK", content_type="text/html", extra_headers=None):
    """
    Génère les headers CGI conformes à la RFC 3875
    Le serveur convertira le Status en ligne de statut HTTP/1.1
    """
    headers = []
    
    # Status (optionnel - sera converti en "HTTP/1.1 200 OK" par le serveur)
    headers.append(f"Status: {status}")
    
    # Content-Type (obligatoire)
    headers.append(f"Content-Type: {content_type}")
    
    # Headers additionnels
    if extra_headers:
        for key, value in extra_headers.items():
            headers.append(f"{key}: {value}")
    
    # Ligne vide obligatoire pour séparer header et body
    headers.append("")
    headers.append("")
    
    return "\r\n".join(headers)

def get_cgi_environment():
    """Récupère les variables d'environnement CGI définies par RFC 3875"""
    return {
        "REQUEST_METHOD": os.environ.get("REQUEST_METHOD", "GET"),
        "QUERY_STRING": os.environ.get("QUERY_STRING", ""),
        "CONTENT_TYPE": os.environ.get("CONTENT_TYPE", ""),
        "CONTENT_LENGTH": os.environ.get("CONTENT_LENGTH", "0"),
        "SERVER_NAME": os.environ.get("SERVER_NAME", "localhost"),
        "SERVER_PORT": os.environ.get("SERVER_PORT", "80"),
        "SERVER_PROTOCOL": os.environ.get("SERVER_PROTOCOL", "HTTP/1.1"),
        "REMOTE_ADDR": os.environ.get("REMOTE_ADDR", "0.0.0.0"),
        "HTTP_USER_AGENT": os.environ.get("HTTP_USER_AGENT", "Unknown"),
        "SCRIPT_NAME": os.environ.get("SCRIPT_NAME", "/cgi-bin/portal.cgi"),
    }

def parse_query_string(query_string):
    """Parse les paramètres de la query string"""
    params = {}
    if query_string:
        for param in query_string.split("&"):
            if "=" in param:
                key, value = param.split("=", 1)
                params[key] = value
    return params

def generate_html_body(params, env):
    """Génère le body HTML avec le portail cosmique"""
    intensity = params.get("intensity", "1.0")
    theme = params.get("theme", "cosmic")
    
    html = f"""<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Portail Cosmique CGI</title>
    <style>
        * {{ margin: 0; padding: 0; box-sizing: border-box; }}
        body {{ 
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #0a0015 0%, #1a0033 100%);
            color: white;
            overflow: hidden;
        }}
        #portal-container {{
            position: relative;
            width: 100vw;
            height: 100vh;
        }}
        canvas {{
            display: block;
            cursor: none;
        }}
        .header {{
            position: absolute;
            top: 20px;
            left: 50%;
            transform: translateX(-50%);
            background: rgba(0, 0, 0, 0.6);
            padding: 15px 30px;
            border-radius: 15px;
            border: 2px solid rgba(138, 43, 226, 0.5);
            backdrop-filter: blur(10px);
            z-index: 10;
        }}
        .header h1 {{
            font-size: 2em;
            background: linear-gradient(90deg, #00d4ff, #9d4edd, #ff006e);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            background-clip: text;
        }}
        .cgi-info {{
            position: absolute;
            bottom: 20px;
            left: 20px;
            background: rgba(0, 0, 0, 0.7);
            padding: 15px;
            border-radius: 10px;
            border: 1px solid rgba(0, 212, 255, 0.3);
            font-size: 0.85em;
            max-width: 400px;
            z-index: 10;
        }}
        .cgi-info h3 {{
            color: #00d4ff;
            margin-bottom: 10px;
        }}
        .cgi-info p {{
            margin: 5px 0;
            color: #aaa;
        }}
        .controls {{
            position: absolute;
            top: 20px;
            right: 20px;
            background: rgba(0, 0, 0, 0.7);
            padding: 15px;
            border-radius: 10px;
            border: 1px solid rgba(157, 78, 221, 0.3);
            z-index: 10;
        }}
        .controls button {{
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            border: none;
            color: white;
            padding: 10px 20px;
            margin: 5px;
            border-radius: 5px;
            cursor: pointer;
            font-weight: bold;
        }}
        .controls button:hover {{
            transform: scale(1.05);
        }}
    </style>
</head>
<body>
    <div id="portal-container">
        <div class="header">
            <h1>✨ PORTAIL COSMIQUE CGI ✨</h1>
        </div>
        
        <canvas id="cosmicCanvas"></canvas>
        
        <div class="cgi-info">
            <h3>📡 CGI Headers (RFC 3875)</h3>
            <p><strong>Status:</strong> 200 OK</p>
            <p><strong>Content-Type:</strong> text/html</p>
            <p><strong>Method:</strong> {env['REQUEST_METHOD']}</p>
            <p><strong>Protocol:</strong> {env['SERVER_PROTOCOL']}</p>
            <p><strong>Client IP:</strong> {env['REMOTE_ADDR']}</p>
            <p><strong>Query:</strong> {env['QUERY_STRING'] or 'none'}</p>
            <p><strong>Generated:</strong> {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
        </div>
        
        <div class="controls">
            <h4 style="color: #9d4edd; margin-bottom: 10px;">Contrôles</h4>
            <button onclick="changeIntensity(0.5)">🌙 Calme</button>
            <button onclick="changeIntensity(1.5)">⚡ Intense</button>
            <button onclick="changeIntensity(3.0)">💥 Explosion</button>
        </div>
    </div>

    <script>
        const canvas = document.getElementById('cosmicCanvas');
        const ctx = canvas.getContext('2d');
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
        
        let mouseX = canvas.width / 2;
        let mouseY = canvas.height / 2;
        let time = 0;
        let intensity = {intensity};
        let particles = [];
        
        class Particle {{
            constructor() {{
                this.x = Math.random() * canvas.width;
                this.y = Math.random() * canvas.height;
                this.vx = (Math.random() - 0.5) * 2;
                this.vy = (Math.random() - 0.5) * 2;
                this.size = Math.random() * 3 + 1;
                this.hue = Math.random() * 360;
            }}
            
            update() {{
                const dx = mouseX - this.x;
                const dy = mouseY - this.y;
                const dist = Math.sqrt(dx * dx + dy * dy);
                
                if (dist < 300) {{
                    const force = (300 - dist) / 300 * 0.5 * intensity;
                    this.vx += (dx / dist) * force;
                    this.vy += (dy / dist) * force;
                }}
                
                this.vx *= 0.95;
                this.vy *= 0.95;
                this.x += this.vx;
                this.y += this.vy;
                
                if (this.x < 0) this.x = canvas.width;
                if (this.x > canvas.width) this.x = 0;
                if (this.y < 0) this.y = canvas.height;
                if (this.y > canvas.height) this.y = 0;
            }}
            
            draw() {{
                const gradient = ctx.createRadialGradient(this.x, this.y, 0, this.x, this.y, this.size * 3);
                gradient.addColorStop(0, `hsla(${{(this.hue + time * 20) % 360}}, 100%, 70%, 0.8)`);
                gradient.addColorStop(1, `hsla(${{(this.hue + time * 20) % 360}}, 100%, 50%, 0)`);
                ctx.fillStyle = gradient;
                ctx.beginPath();
                ctx.arc(this.x, this.y, this.size * 3, 0, Math.PI * 2);
                ctx.fill();
            }}
        }}
        
        for (let i = 0; i < 100; i++) {{
            particles.push(new Particle());
        }}
        
        function drawVortex() {{
            for (let spiral = 0; spiral < 6; spiral++) {{
                const offset = (spiral / 6) * Math.PI * 2;
                ctx.beginPath();
                
                for (let i = 0; i < 80; i++) {{
                    const t = i / 80;
                    const angle = t * Math.PI * 5 + offset + time * 0.5;
                    const radius = t * 250 * intensity;
                    const x = mouseX + Math.cos(angle) * radius;
                    const y = mouseY + Math.sin(angle) * radius;
                    
                    if (i === 0) ctx.moveTo(x, y);
                    else ctx.lineTo(x, y);
                }}
                
                const hue = (time * 50 + spiral * 60) % 360;
                ctx.strokeStyle = `hsla(${{hue}}, 100%, 60%, 0.6)`;
                ctx.lineWidth = 2 * intensity;
                ctx.stroke();
            }}
        }}
        
        function drawRings() {{
            for (let i = 0; i < 4; i++) {{
                const radius = 50 + i * 60 + Math.sin(time * 2 + i) * 15;
                const hue = (time * 30 + i * 90) % 360;
                ctx.beginPath();
                ctx.arc(mouseX, mouseY, radius * intensity, 0, Math.PI * 2);
                ctx.strokeStyle = `hsla(${{hue}}, 100%, 70%, 0.4)`;
                ctx.lineWidth = 2;
                ctx.stroke();
            }}
        }}
        
        function animate() {{
            time += 0.016;
            
            const gradient = ctx.createRadialGradient(mouseX, mouseY, 0, mouseX, mouseY, canvas.width);
            gradient.addColorStop(0, `hsla(${{(time * 10) % 360}}, 40%, 5%, 1)`);
            gradient.addColorStop(1, `hsla(${{(time * 10 + 180) % 360}}, 60%, 2%, 1)`);
            ctx.fillStyle = gradient;
            ctx.fillRect(0, 0, canvas.width, canvas.height);
            
            particles.forEach(p => {{
                p.update();
                p.draw();
            }});
            
            drawRings();
            drawVortex();
            
            const centerGradient = ctx.createRadialGradient(mouseX, mouseY, 0, mouseX, mouseY, 80 * intensity);
            centerGradient.addColorStop(0, `hsla(${{(time * 50) % 360}}, 100%, 80%, 0.8)`);
            centerGradient.addColorStop(1, 'rgba(0, 0, 0, 0)');
            ctx.fillStyle = centerGradient;
            ctx.beginPath();
            ctx.arc(mouseX, mouseY, 80 * intensity, 0, Math.PI * 2);
            ctx.fill();
            
            requestAnimationFrame(animate);
        }}
        
        canvas.addEventListener('mousemove', (e) => {{
            mouseX = e.clientX;
            mouseY = e.clientY;
        }});
        
        canvas.addEventListener('click', () => {{
            intensity = 2.5;
            setTimeout(() => intensity = 1, 500);
        }});
        
        function changeIntensity(val) {{
            intensity = val;
        }}
        
        animate();
    </script>
</body>
</html>"""
    return html

def main():
    """Point d'entrée principal du CGI"""
    try:
        # Récupérer l'environnement CGI
        env = get_cgi_environment()
        
        # Parser les paramètres
        params = parse_query_string(env["QUERY_STRING"])
        
        # Générer le header CGI (RFC 3875)
        header = generate_header(
            status="200 OK",
            content_type="text/html; charset=utf-8",
            extra_headers={
                "Cache-Control": "no-cache",
                "X-Powered-By": "Python CGI/RFC3875",
                "X-Generator": "Cosmic Portal CGI v1.0"
            }
        )
        
        # Générer le body
        body = generate_html_body(params, env)
        
        # Écrire la réponse sur stdout (comme requis par CGI)
        sys.stdout.write(header)
        sys.stdout.write(body)
        sys.stdout.flush()
        
    except Exception as e:
        # En cas d'erreur, retourner un statut 500
        error_header = generate_header(
            status="500 Internal Server Error",
            content_type="text/html; charset=utf-8"
        )
        error_body = f"""<!DOCTYPE html>
<html>
<head><title>500 Error</title></head>
<body>
    <h1>500 Internal Server Error</h1>
    <p>Une erreur s'est produite dans le script CGI:</p>
    <pre>{str(e)}</pre>
</body>
</html>"""
        
        sys.stdout.write(error_header)
        sys.stdout.write(error_body)
        sys.stdout.flush()

if __name__ == "__main__":
    main()