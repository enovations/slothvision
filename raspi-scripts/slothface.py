import numpy as np
import math
import sys
import rospy
import tf
import socket
from geometry_msgs.msg import Twist, Transform, Pose, PoseStamped, Point, Point32, PointStamped, Vector3, Quaternion
from amsc_common.msg import PowerSupply
from sensor_msgs.msg import PointCloud
from tf import TransformListener
from tf.transformations import euler_from_quaternion

SEND_IP = ""
SEND_PORT = -1

UDP_IP = "192.168.0.100"
UDP_PORT = 8008

send_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)


fixFrame = "/map"
laserFrame = "/robot"

tf_listener = []

def battery(data):
    if SEND_IP != "" and SEND_PORT != -1:
        to_send = 'battery: ' + str(data.batteries[0].voltage)
        send_sock.sendto(to_send, (SEND_IP, SEND_PORT))

def sonar(data):
    if SEND_IP != "" and SEND_PORT != -1:
        send_sock.sendto('sonar new', (SEND_IP, SEND_PORT))
        for point in data.points:
            to_send = 'sonar: ' + str(point.x) + ' ' + str(point.y)
            send_sock.sendto(to_send, (SEND_IP, SEND_PORT))


def talker():
    global tf_listener
    global send_sock
    global SEND_IP
    global SEND_SOCK

    rospy.init_node('slothface', anonymous=True)

    tf_listener = TransformListener()

    sock = socket.socket(socket.AF_INET,
                         socket.SOCK_DGRAM)
    sock.setblocking(0)
    sock.bind((UDP_IP, UDP_PORT))

    robot_pub = rospy.Publisher('/robot/cmd_vel', Twist, queue_size=0)
    gimbalo_pub = rospy.Publisher('/pantilt/cmd_vel', Twist, queue_size=0)

    rospy.Subscriber('/robot/power_supply', PowerSupply, battery)
    rospy.Subscriber('/robot/sonar', PointCloud, sonar)

    rate = rospy.Rate(100)
    while not rospy.is_shutdown():
        try:
            data, addr = sock.recvfrom(1024)
        except:
            data = 'no data'
        string = data.decode('utf-8')
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

        try:
            if tf_listener.frameExists(laserFrame) and tf_listener.frameExists(fixFrame):
                t = tf_listener.getLatestCommonTime(laserFrame, fixFrame)
                position, quaternion = tf_listener.lookupTransform(fixFrame, laserFrame, t)
                (roll,pitch,yaw) = euler_from_quaternion(quaternion)
                if SEND_IP != "" and SEND_PORT != -1:
                    to_send = 'position: ' + str(position[0]) + ' ' + str(position[1]) + ' ' + str(yaw)
                    send_sock.sendto(to_send, (SEND_IP, SEND_PORT))
        except:
            print(sys.exc_info()[0])
        rate.sleep()

if __name__ == '__main__':
    talker()
