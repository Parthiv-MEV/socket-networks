import socket
import struct
import sys
import time


def read_graph(path):
    rows = []
    with open(path) as file:
        for line in file:
            line = line.split('#')[0].strip()
            if line:
                rows.append([int(x) for x in line.split()])
    n = len(rows) + 1
    graph = [[-1] * n for i in range(n)]
    for i in range(n):
        graph[i][i] = 0
    for i, row in enumerate(rows):
        for offset, cost in enumerate(row):
            j = i + offset + 1
            graph[i][j] = cost
            graph[j][i] = cost
    return graph


def receive(sock, n):
    data = b''
    while len(data) < n:
        chunk = sock.recv(n - len(data))
        if not chunk:
            print('Error 1')
        data += chunk
    return data


def send_neighbours(graph, nodes):
    for i, (conn, ipaddr, udpport) in enumerate(nodes):
        entries = []
        for j, (conn, ipaddr, udpport) in enumerate(nodes):
            if graph[i][j] != -1:
                entries.append(bytes([65+j])+ipaddr+udpport.to_bytes(2, 'big') + graph[i][j].to_bytes(2, 'big'))
        body = bytes([2]) + len(entries).to_bytes(2, 'big') + b''.join(entries)
        conn.sendall(len(body).to_bytes(2, 'big') + body)
        print(f'Sent neighbours to {chr(65 + i)}', flush=True)


def main():
    if len(sys.argv) != 3:
        print('Usage: python3 oracle.py <oracle-ip> <config-file>')
        return

    ip, config = sys.argv[1:]
    graph = read_graph(config)
    n = len(graph)
    nodes = []

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        server.bind((ip, 5000))
        server.listen()
        print(f'I\'m waiting', flush=True)

        while len(nodes) < n:
            conn, _ = server.accept()
            msg = receive(conn, 9)
            vn_ip = msg[3:7]
            vn_port = int.from_bytes(msg[7:9], 'big')
            nodes.append((conn, vn_ip, vn_port))
            print(f'{chr(64 + len(nodes))} connected: {socket.inet_ntoa(vn_ip)}:{vn_port}', flush=True)

        send_neighbours(graph, nodes)

        while True:
            time.sleep(1)
            updated = read_graph(config)
            if len(updated) == n and updated != graph:
                graph = updated
                print('File changed', flush=True)
                send_neighbours(graph, nodes)


if __name__ == '__main__':
    main()
