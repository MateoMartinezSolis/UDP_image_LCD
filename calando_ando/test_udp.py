import socket, time
UDP_IP = "192.168.0.102"
UDP_PORT = 7
#MESSAGE = b"calando ando"
MESSAGE =  b'\x55'
TIME = 2

print("UPP target IP: %s" % UDP_IP)
print("UDP target port: %s" % UDP_PORT)

sock = socket.socket(socket.AF_INET, #Internet
                     socket.SOCK_DGRAM) #UDP

sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))