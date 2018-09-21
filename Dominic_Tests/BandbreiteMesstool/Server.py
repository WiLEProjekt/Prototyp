import socket

def main():
    myip = "127.0.0.1"
    myport = 50002
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind((myip, myport))
    while True:
        sock.listen()
        conn, addr = sock.accept()
        data = conn.recv(2048)
        measurementID = data.decode()
        pcapfilename = "server_"+measurementID
        print(pcapfilename)

if __name__ == "__main__":
    main()