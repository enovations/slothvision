import socket
import rospy
from geometry_msgs.msg import Twist
from geometry_msgs.msg import Vector3
from amsc_common.msg import PowerSupply

SEND_IP = ""
SEND_PORT = -1

UDP_IP = "192.168.0.100"
UDP_PORT = 8008

send_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def battery(data):
    print(SEND_IP, SEND_PORT)
    if SEND_IP != "" and SEND_PORT != -1:
        to_send = 'battery: ' + str(data.batteries[0].voltage)
        send_sock.sendto(to_send, (SEND_IP, SEND_PORT))
        print('battery poslan')

def talker():
    sock = socket.socket(socket.AF_INET,
                         socket.SOCK_DGRAM)
    sock.bind((UDP_IP, UDP_PORT))

    robot_pub = rospy.Publisher('/robot/cmd_vel', Twist, queue_size=0)
    gimbalo_pub = rospy.Publisher('/pantilt/cmd_vel', Twist, queue_size=0)
    rospy.init_node('talker', anonymous=True)

    rospy.Subscriber('/robot/power_supply', PowerSupply, battery)

    rate = rospy.Rate(100)
    while not rospy.is_shutdown():
        data, addr = sock.recvfrom(1024)
        string = data.decode('utf-8')#[:-1]
        parsed_data = string.split()

        if parsed_data[0] == 'r':
            message = Twist(Vector3(float(parsed_data[1]), 0, 0), Vector3(0, 0, float(parsed_data[2])))
            robot_pub.publish(message)
        if parsed_data[0] == 'g':
            message = Twist(Vector3(float(parsed_data[1]), 0, 0), Vector3(0, 0, float(parsed_data[2])))
            gimbalo_pub.publish(message)

        if parsed_data[0] == "port:":
            global SEND_PORT
            SEND_PORT = int(parsed_data[1])
            print('port setan')
        if parsed_data[0] == "ip:":
            global SEND_IP
            SEND_IP = parsed_data[1]
            print('ip setan')
        rate.sleep()

if __name__ == '__main__':
    talker()
