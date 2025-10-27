# import os
# import sys
# import json
# from urllib.parse import parse_qs

# # Function to print CGI header
# def print_header():
#     print("Content-Type: text/plain\n")

# # Default values
# default_values = {'value1': 'default1', 'value2': 'default2'}

# # Read QUERY_STRING
# query_string = os.environ.get('QUERY_STRING', '')

# if query_string:
#     # If GET, parse query string
#     params = parse_qs(query_string)
#     # Extract first value from list
#     data = {k: v[0] for k, v in params.items()}
# else:
#     # Try to read POST JSON from stdin
#     try:
#         content_length = int(os.environ.get('CONTENT_LENGTH', 0))
#     except (ValueError, TypeError):
#         content_length = 0

#     if content_length > 0:
#         raw_post = sys.stdin.read(content_length)
#         try:
#             data = json.loads(raw_post)
#         except json.JSONDecodeError:
#             data = default_values
#     else:
#         # Fallback to default
#         data = default_values

# # Output
# print_header()
# print(f"value1 = {data.get('value1')}")
# print(f"value2 = {data.get('value2')}")
count = 0
while (1):
    count+=1