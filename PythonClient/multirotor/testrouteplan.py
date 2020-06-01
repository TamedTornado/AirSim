import setup_path 
from airsim.routeplan import RoutePlan



rp = RoutePlan()

rp.loadFromFile("D:/JasonsDocs/AirSim/Tester.rpjson")

print("Successful load")
print(rp.Nodes)