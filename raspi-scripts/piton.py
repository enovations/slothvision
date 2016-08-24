import socket
import subprocess

UDP_IP = "192.168.0.50"
UDP_PORT = 8008

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))

ip = "192.168.0.105"
port = "5000"

while True:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    stringus = data.decode("utf-8")
    stringus = stringus[:-1]
    if stringus[:6] == "port: ":
      port = stringus[6:]
      print("port nastaulen na", port)
    elif stringus[:4] == "ip: ":
      ip = stringus[4:] 
      print("ip nastaulen na", ip)
    elif stringus == "run":
      bashCommand = "/bin/bash /opt/skripta_za_pognt_ob_bootu_z_zelo_zelooo_dolgimi_imenom.sh " + ip + " " + port + " &"
      process = subprocess.Popen(bashCommand.split(), stdout=subprocess.PIPE)
    elif stringus == "kill":
      bashCommand = "killall raspivid"
      process = subprocess.Popen(bashCommand.split(), stdout=subprocess.PIPE)
    else:
      print("uknown komandus!")
