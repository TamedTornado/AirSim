import setup_path 
import airsim

import sys
import time

from airsim.routeplan import RoutePlan

# Load the route plan!
rp = RoutePlan()

rp.loadFromFile("D:/JasonsDocs/AirSim/TestGimbals.rpjson")

print("Successful load")

# Assemble the path into a list for use later

pathCoords = []
RootPosition = rp.RootPosition

for node in rp.Nodes:

	nodeCoord = node['Coord']

	# The RootPosition of the RoutePlan is the position the drone is on while on the ground. All coords are relative to that position.
	coord = airsim.Vector3r(RootPosition.x_val + nodeCoord.x_val, RootPosition.y_val + nodeCoord.y_val, RootPosition.z_val + nodeCoord.z_val)
	pathCoords.append(coord)

# Now let's fly it

client = airsim.MultirotorClient()
client.confirmConnection()
client.enableApiControl(True)

print("arming the drone...")
client.armDisarm(True)

state = client.getMultirotorState()
if state.landed_state == airsim.LandedState.Landed:
    print("taking off...")
    client.takeoffAsync().join()
else:
    client.hoverAsync().join()

time.sleep(1)

state = client.getMultirotorState()
if state.landed_state == airsim.LandedState.Landed:
    print("take off failed...")
    sys.exit(1)

# AirSim uses NED coordinates so negative axis is up.
# z of -7 is 7 meters above the original launch point.
z = -7
print("make sure we are hovering at 7 meters...")
client.moveToZAsync(z, 1).join()

# see https://github.com/Microsoft/AirSim/wiki/moveOnPath-demo

# this method is async and we are not waiting for the result since we are passing timeout_sec=0.

print("flying on path...")
result = client.moveOnPathAsync(pathCoords,
                        12, 120,
                        airsim.DrivetrainType.ForwardOnly, airsim.YawMode(False,0), 20, 1).join()

# drone will over-shoot so we bring it back to the start point before landing.
client.moveToPositionAsync(0,0,z,1).join()
print("landing...")
client.landAsync().join()
print("disarming...")
client.armDisarm(False)
client.enableApiControl(False)
print("done.")
