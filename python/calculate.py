import os
from dotenv import load_dotenv
import datetime

x = datetime.datetime.now()

load_dotenv()

MY_ENV_VAR = os.getenv('QUERY_STRING')


print("HTTP/1.1 200 OK")
print(x.strftime("Date: %a, %w %b %Y %X %Z"))
print("Server: Webserv")
print("Content-Type: text/html; charset=UTF-8")
print("Content-Length: 1234")
print(MY_ENV_VAR)