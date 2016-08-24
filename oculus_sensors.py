import time
from ovrsdk import *
import sys

ovr_Initialize()
hmd = ovrHmd_Create(0)
hmdDesc = ovrHmdDesc()
ovrHmd_GetDesc(hmd, byref(hmdDesc))
ovrHmd_StartSensor( \
  hmd,
  ovrSensorCap_Orientation |
  ovrSensorCap_YawCorrection,
  0
)

if len(sys.argv) == 1:
	sleep_time = 1/float(50)
	print("setting default frequency to 50hZ, add frequency as first arg to set it")
else:
	sleep_time = 1/float(sys.argv[1])
	print("frequency set to "+str(sys.argv[1]))

while True:
	time.sleep(sleep_time)
	ss = ovrHmd_GetSensorState(hmd, ovr_GetTimeInSeconds())
	pose = ss.Predicted.Pose
	print(str(pose.Orientation.w) + ";" + str(pose.Orientation.x) + ";" + str(pose.Orientation.y) + ";" + str(pose.Orientation.z))

