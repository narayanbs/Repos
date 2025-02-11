# Handle post request
import json
from http.server import BaseHTTPRequestHandler, HTTPServer
from urllib.parse import parse_qs


class SimpleHandler(BaseHTTPRequestHandler):

    def do_POST(self):
        # Read the content length from the request headers
        content_length = int(self.headers["Content-Length"])
        # Read the body data
        post_data = self.rfile.read(content_length)

        # Parse the form data
        parsed_data = parse_qs(post_data.decode("utf-8"))

        # You can access form fields as a dictionary
        # Example: form_field = parsed_data.get('field_name', [None])[0]

        # Print the parsed data (for demonstration purposes)
        print(parsed_data)

        # Send response headers
        self.send_response(200)
        self.send_header("Content-type", "application/json")
        self.end_headers()

        # Send back the received POST data (or any other response)
        response = {"received_data": parsed_data}
        self.wfile.write(json.dumps(response).encode("utf-8"))


# Set up and run the server
def run(server_class=HTTPServer, handler_class=SimpleHandler, port=8080):
    server_address = ("", port)
    httpd = server_class(server_address, handler_class)
    print(f"Starting server on port {port}...")
    httpd.serve_forever()


if __name__ == "__main__":
    run()
