import json, sys
from .types import Vector3r
from enum import Enum
from json import JSONDecodeError

class ELastNodeBehaviour(Enum):
	LandAtLastNode = 0
	LandAtStartNode = 1
	ReturnToStartAndLoop = 2

class RoutePlan:

	Name="No Name"
	Filename="NoFileName.rpjson"
	RootPosition = Vector3r()
	DefaultAltitude = 10
	DefaultVelocity = 20

	# A dictionary storing the default gimbal orientation by camera name as well as the info for that camera
	Gimbals = {}

	LastNodeBehaviour = ELastNodeBehaviour.LandAtLastNode

	# Nodes is a list of dicts with the node data
	Nodes = []

	# Read JSON into a Vector3r and return it
	def readVector(self, json):
		result = Vector3r()

		result.x_val = json['X']
		result.y_val = json['Y']
		result.z_val = json['Z']

		return result

	# Read Rotator JSON into a Vector3r and return it	
	def readRotatorToVector(self, json):
		result = Vector3r()

		result.x_val = json['Roll']
		result.y_val = json['Pitch']
		result.z_val = json['Yaw']

		return result

	# Load route plan from filename
	def loadFromFile(self, jsonFilename):

		jsonStr=""

		try:

			with open(jsonFilename) as f:
				jsonStr=f.read()

		except FileNotFoundError:
			print("Failed to open route plan file %s" % jsonFilename, file=sys.stderr)
			return False

		try:
			rpJson = json.loads(jsonStr)
		except JSONDecodeError as de:
			print("Failed to decode JSON file %s" % jsonFilename, file=sys.stderr)
			print("Error: %s at Line %d Col %d" % (de.msg, de.lineno, de.colno), file=sys.stderr)
			return False

		# Sanity check that this json is actually a routeplan

		if 'RootPosition' not in rpJson:
			print("JSON file %s doesn't appear to be a route plan file" % jsonFilename, file=sys.stderr)
			return False

		# Read in the top level data

		self.name = rpJson['Name']
		self.Filename = jsonFilename;

		self.RootPosition = self.readVector(rpJson['RootPosition'])
		self.DefaultVelocity = rpJson['DefaultVelocity']
		self.DefaultAltitude = rpJson['DefaultAltitude']

		# Read in the gimbals defaults and camera data

		for key, gimbal in rpJson['Gimbals'].items():
			newEntry = {}

			self.Gimbals[key] = newEntry;

			newEntry['DefaultOrientation'] = self.readRotatorToVector(gimbal['DefaultOrientation'])

			# NOTE: Maybe this should be another object.
			newEntry['GimbalCameraData'] = gimbal['GimbalCameraData']


		self.LastNodeBehaviour = ELastNodeBehaviour(rpJson['LastNodeBehaviour'])

		# Now the nodes

		for node in rpJson['Nodes']:

			# Convert coord from a dict to a Vector3r object in place
			node['Coord'] = self.readVector(node['Coord'])

			# Convert the gimbal orientations to Vector3r objects in place. 
			for key, gimbal in node['Gimbals'].items():
				gimbal['Orientation'] = self.readRotatorToVector(gimbal['Orientation'])

			# leave the rest alone and store it.

			self.Nodes.append(node)


		
		



