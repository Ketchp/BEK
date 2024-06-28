#!/usr/bin/env python3
import sys
import socket
import select


_LOCALHOST = '127.0.0.1'


def main():
    if len(sys.argv) > 1:
        port = int(sys.argv[1])
    else:
        port = 12345

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    sock.connect((_LOCALHOST, port))
    sock.setblocking(False)

    print("Connected")

    ep = select.epoll()
    ep.register(sock.fileno(), select.EPOLLIN | select.EPOLLERR | select.EPOLLHUP | select.EPOLLRDHUP)

    while True:
        try:
            data = input() + '\n'
        except EOFError:
            break
        sock.send(data.encode())

        response = ''
        while '\n' not in response:
            (fd, event), = ep.poll(maxevents=1)
            if event & (select.EPOLLERR | select.EPOLLHUP | select.EPOLLRDHUP):
                print("Error")
                return

            response += sock.recv(1024).decode()

        print(response)

    sock.close()


if __name__ == '__main__':
    main()
