#!/usr/bin/env python3

import os
import sys
import json
import uuid
from http.cookies import SimpleCookie
from urllib.parse import parse_qs

DB = "quiz.db"

QUESTIONS = {
    1: ("What is 2 + 2?", "4"),
    2: ("What is the capital of France?", "paris"),
    3: ("What color is the sky on a clear day?", "blue"),
    4: ("How many days are there in a week?", "7"),
    5: ("What is the largest planet in our solar system?", "jupiter"),
    6: ("What language is this CGI script written in?", "python"),
    7: ("What is 9 * 8?", "72"),
    8: ("Which ocean is the largest?", "pacific"),
    9: ("What is the chemical symbol for water?", "h2o"),
    10: ("What is the first letter of the English alphabet?", "a")
}

# ----------------------------------------------------
# Load database
# ----------------------------------------------------

if os.path.exists(DB):
    with open(DB, "r") as f:
        users = json.load(f)
else:
    users = {}

# ----------------------------------------------------
# Read cookies
# ----------------------------------------------------

cookie = SimpleCookie()
cookie.load(os.environ.get("HTTP_COOKIE", ""))

new_user = False

if "player" in cookie:
    player = cookie["player"].value
else:
    player = "guest-" + uuid.uuid4().hex[:8]
    new_user = True
    users[player] = 1

level = users.get(player, 1)

# ----------------------------------------------------
# Handle POST request
# ----------------------------------------------------

message = ""

if os.environ.get("REQUEST_METHOD") == "POST":

    length = int(os.environ.get("CONTENT_LENGTH", "0"))
    body = sys.stdin.read(length)

    params = parse_qs(body)
    answer = params.get("answer", [""])[0].strip().lower()

    if level in QUESTIONS:

        correct_answer = QUESTIONS[level][1]

        if answer == correct_answer:

            level += 1
            users[player] = level

            with open(DB, "w") as f:
                json.dump(users, f, indent=4)

            message = "<p style='color:green'><b>Correct!</b></p>"

        else:
            message = (
                "<p style='color:red'><b>Wrong answer.</b> "
                "Please try again.</p>"
            )

# ----------------------------------------------------
# HTTP Response
# ----------------------------------------------------

sys.stdout.write("Content-Type: text/html\r\n")

if new_user:
    sys.stdout.write(f"Set-Cookie: player={player}; Path=/\r\n")

sys.stdout.write("\r\n")

# ----------------------------------------------------
# HTML
# ----------------------------------------------------

sys.stdout.write("""<!DOCTYPE html>
<html>
<head>
<title>CGI Cookie Quiz</title>
<style>
body {
    font-family: Arial, sans-serif;
    margin: 40px;
}

input[type=text] {
    padding: 8px;
    width: 250px;
}

input[type=submit] {
    padding: 8px 16px;
}
</style>
</head>
<body>
""")

sys.stdout.write("<h1>CGI Cookie Quiz</h1>")
sys.stdout.write(f"<p><b>Player:</b> {player}</p>")

if level <= len(QUESTIONS):
    sys.stdout.write(
        f"<p><b>Level:</b> {level} / {len(QUESTIONS)}</p>"
    )
else:
    sys.stdout.write(
        f"<p><b>Level:</b> {len(QUESTIONS)} / {len(QUESTIONS)}</p>"
    )

sys.stdout.write(message)

if level <= len(QUESTIONS):

    question = QUESTIONS[level][0]

    sys.stdout.write(f"""
<form method="POST">
    <p><b>Question {level}:</b></p>
    <p>{question}</p>

    <input type="text" name="answer" autofocus>
    <br><br>

    <input type="submit" value="Submit Answer">
</form>
""")

else:

    sys.stdout.write("""
<h2>🎉 Congratulations!</h2>
<p>You completed all 10 levels.</p>
<p>Your progress is stored in <code>quiz.db</code>.</p>
<p>Delete your browser cookie or the database entry to start over.</p>
""")

sys.stdout.write("""
</body>
</html>
""")
