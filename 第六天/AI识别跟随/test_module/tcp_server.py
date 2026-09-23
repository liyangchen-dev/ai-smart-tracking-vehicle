import socket

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = ('localhost', 10001)
server_socket.bind(server_address)
server_socket.listen(1)

while True:
    client_socket, client_address = server_socket.accept()
    print(f"连接地址: {client_address}")
    try:
        while True:
            data = client_socket.recv(1024)
            if data:
                print(data)
                client_socket.sendall(data)
            else:
                break
    finally:
        client_socket.close()
