import select
import socket
import sys

# Function to broadcast messages to all connected clients
def broadcast_message(sock, message):
    for socket in connected_clients:
        # Send the message only to peers (not the server or the sender)
        if socket != server_socket and socket != sock:
            try:
                socket.send(message)
            except:
                # If sending fails, assume the connection is broken
                socket.close()
                connected_clients.remove(socket)

# Set up the server socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

# Bind the server socket to an address and port
server_address = ('', 12345)
server_socket.bind(server_address)

# Listen for incoming connections
server_socket.listen(10)

# List to keep track of socket objects
connected_clients = [server_socket]

print("Server is running on port 12345...")

while True:
    # Use select to wait until there is data to read or connections to accept
    read_sockets, _, exception_sockets = select.select(connected_clients, [], connected_clients)

    for sock in read_sockets:
        # If the server socket is ready, it means there is a new connection to accept
        if sock == server_socket:
            client_socket, client_address = server_socket.accept()
            connected_clients.append(client_socket)
            print(f"New connection from {client_address}")

            # Send a welcome message to the client
            client_socket.send("Welcome to the chat server!\n".encode())

            # Broadcast to other clients that a new client has joined
            broadcast_message(client_socket, f"Client {client_address} has joined the chat.\n".encode())

        # Otherwise, it's a client socket that has data to be read
        else:
            try:
                # Receive data from the client
                data = sock.recv(4096)
                if data:
                    message = data.decode()
                    # Broadcast the received message to all clients
                    broadcast_message(sock, f"Client {sock.getpeername()}: {message}".encode())
                else:
                    # If no data received, client has closed the connection
                    print(f"Connection closed from {sock.getpeername()}")
                    # Broadcast to other clients that this client has disconnected
                    broadcast_message(sock, f"Client {sock.getpeername()} has left the chat.\n".encode())
                    # Close the socket and remove it from the list of connected clients
                    sock.close()
                    connected_clients.remove(sock)
            except:
                # Error encountered, likely the client has disconnected abruptly
                print(f"Connection error from {sock.getpeername()}")
                broadcast_message(sock, f"Client {sock.getpeername()} has left the chat due to an error.\n".encode())
                sock.close()
                connected_clients.remove(sock)

    # Handle exceptional conditions
    for sock in exception_sockets:
        print(f"Exceptional condition on {sock.getpeername()}")
        # Close the socket and remove it from the list of connected clients
        sock.close()
        connected_clients.remove(sock)
