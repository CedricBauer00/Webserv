#!/usr/bin/env python3

import os
import sys

method = os.environ.get("REQUEST_METHOD", "")

if method == "GET":
    print("Content-Type: text/plain\r")
    print("\r")

    print("Set-Cookie: session_id=abc123xyz789; HttpOnly; Path=/;\r")
    print("Set-Cookie: user_pref=dark_mode; Path=/;\r")

    print(f"Method: {method}\r")
    print(f"Script: {os.environ.get('SCRIPT_NAME', '')}\r")
    print(f"Query:  {os.environ.get('QUERY_STRING', '')}\r")
    print(f"Client: {os.environ.get('REMOTE_ADDR', '')}\r")
    print(f"Host:   {os.environ.get('SERVER_NAME', '')}\r")
    print(f"Port:   {os.environ.get('SERVER_PORT', '')}\r")
    print(f"Proto:  {os.environ.get('SERVER_PROTOCOL', '')}\r")
    print(f"CGI:    {os.environ.get('GATEWAY_INTERFACE', '')}\r")

    input_data = sys.stdin.read()

    print("------BODY FOLLOWS-------")
    print(input_data)

elif method == "POST":
    print("Status: 302 Found\r")
    print("Location: /get/\r")
    print("Set-Cookie: session_id=new_session_456; HttpOnly; Path=/")
    print("\r")

elif method == "DELETE":
    print("Status: 403 Forbidden\r")
    print("Content-Type: text/plain\r")
    print("\r")

else:
    print("Status: 405 Method Not Allowed\r")
    print("\r")