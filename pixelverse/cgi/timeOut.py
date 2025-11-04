#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import json
from datetime import datetime
from urllib.parse import parse_qs
from io import BytesIO

def parse_multipart(content_type, content_length):
    """Parse multipart form data without deprecated cgi module"""
    data = {}
    
    if 'multipart/form-data' in content_type:
        # Simple form data parsing - read from stdin
        if content_length:
            body = sys.stdin.buffer.read(int(content_length))
            # For basic implementation, just note that multipart data was received
            data['_multipart'] = f"[Received {len(body)} bytes of multipart data]"
    else:
        # URL-encoded form data
        if content_length:
            body = sys.stdin.read(int(content_length))
            data = parse_qs(body)
            # Flatten single-value lists
            data = {k: v[0] if len(v) == 1 else v for k, v in data.items()}
    
    return data

def get_cgi_env():
    """Retrieve CGI environment variables"""
    return {
        'method': os.environ.get('REQUEST_METHOD', ''),
        'query': os.environ.get('QUERY_STRING', ''),
        'content_type': os.environ.get('CONTENT_TYPE', ''),
        'content_length': os.environ.get('CONTENT_LENGTH', '0'),
        'server_name': os.environ.get('SERVER_NAME', 'localhost'),
        'server_port': os.environ.get('SERVER_PORT', '80'),
        'script_name': os.environ.get('SCRIPT_NAME', ''),
        'remote_addr': os.environ.get('REMOTE_ADDR', ''),
        'http_user_agent': os.environ.get('HTTP_USER_AGENT', ''),
    }

def format_irc_response(code, message, data=None):
    """Format response in IRC-style notation"""
    timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    server = os.environ.get('SERVER_NAME', 'server')
    response = f":{server} {code} * :{message}\r\n"
    
    if data:
        for key, value in data.items():
            response += f":{server} DATA * :{key}={value}\r\n"
    
    return response

def handle_get(params):
    """Handle GET request"""
    parsed = parse_qs(params) if params else {}
    
    # Flatten single-value lists
    parsed = {k: v[0] if len(v) == 1 else ','.join(v) for k, v in parsed.items()}
    
    response = format_irc_response("200", "GET request processed successfully")
    
    if parsed:
        response += format_irc_response("INFO", "Query parameters received", parsed)
    else:
        response += format_irc_response("INFO", "No query parameters provided")
    
    return response

def handle_post(content_type, content_length):
    """Handle POST request"""
    data = parse_multipart(content_type, content_length)
    
    response = format_irc_response("200", "POST request processed successfully")
    
    if data:
        response += format_irc_response("INFO", "Form data received", data)
    else:
        response += format_irc_response("INFO", "No form data provided")
    
    return response

def main():
    """Main CGI handler"""
    env = get_cgi_env()
    
    # Send HTTP headers (CGI/1.1 specification)
    print("Content-Type: text/plain; charset=utf-8")
    print("Status: 200 OK")
    print()  # Empty line separates headers from body
    
    # IRC-style server banner
    print(f":{env['server_name']} 001 * :Welcome to CGI/1.1 IRC-Style Handler")
    print(f":{env['server_name']} 002 * :Server {env['server_name']}:{env['server_port']}")
    print(f":{env['server_name']} 003 * :Remote {env['remote_addr']}")
    print()
    
    # Route based on request method
    if env['method'] == 'GET':
        print(handle_get(env['query']))
    elif env['method'] == 'POST':
        print(handle_post(env['content_type'], env['content_length']))
    else:
        print(format_irc_response("405", f"Method {env['method']} not allowed"))
    
    # Footer
    print()
    print(f":{env['server_name']} 250 * :End of response")
    print(f":{env['server_name']} TIME * :{datetime.now().isoformat()}")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print("Content-Type: text/plain; charset=utf-8")
        print("Status: 500 Internal Server Error")
        print()
        print(format_irc_response("500", f"Internal Server Error: {str(e)}"))
        sys.exit(1)