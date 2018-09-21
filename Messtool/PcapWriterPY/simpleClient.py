import socket
import time


def main():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(("127.0.0.1", 84))
    time.sleep(2)
    sock.send("jo".encode())
    time.sleep(2)
    sock.close()


if __name__ == "__main__":
    main()
