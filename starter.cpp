#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

enum Floors {
  MID = 2500 / 2,
  F0 = 0,
  F1 = 2500,
  F2 = 5000,
  F3 = 7500,
  F4 = 10000,
};

float distance(int x1, int y1, int x2, int y2) {
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) * 1.0);
}
void moveTo(int x, int y, int light, string ctx) {
  // MOVE <x> <y> <light (1|0)> | WAIT <light (1|0)>
  cout << "MOVE " << x << " " << y << " " << light << " " << ctx << endl;
}

/* ========================================================================= */

class Creature {
public:
  int id, color, type;
  int x;
  int y;
  map<int, string> direction;
  int scannedBy;
  bool saved;
  bool alive;
  Creature(int id, int color, int type) {
    this->id = id;
    this->color = color;
    this->type = type;
    x = 0;
    y = 0;
    saved = false;
    scannedBy = -1;
    alive = true;
  }
  Creature(const Creature &copy) { *this = copy; }
  Creature &operator=(const Creature &rhs) {
    id = rhs.id;
    color = rhs.color;
    type = rhs.type;
    x = rhs.x;
    y = rhs.y;
    saved = rhs.saved;
    scannedBy = rhs.scannedBy;
    alive = rhs.alive;
    direction = rhs.direction;
    return *this;
  }
  bool operator==(const Creature &rhs) { return id = rhs.id; }
  bool operator==(const int id) { return this->id = id; }
  bool isTargetable() {
    return !(saved || scannedBy != -1 || type == -1 || !alive);
  }
};
typedef map<int, Creature> Creatures;

class Drone {
public:
  int id;
  int x, y;
  int battery;
  Creature *target;
  Drone(int id, int x, int y, int battery)
      : id(id), x(x), y(y), battery(battery) {
    target = NULL;
  }
  map<string, vector<int>> blip = {
      {"TL", {}}, {"TR", {}}, {"BL", {}}, {"BR", {}}};

  bool operator==(const Drone &rhs) { return id = rhs.id; }
  bool operator==(const int id) { return this->id = id; }
  void move(string d, int light, string ctx) {
    if (d == "TL") {
      moveTo(x - 600, y - 600, light, "go TL " + ctx);
    } else if (d == "TR") {
      moveTo(x + 600, y - 600, light, "go TR " + ctx);
    } else if (d == "BL") {
      moveTo(x - 600, y + 600, light, "go BL " + ctx);
    } else if (d == "BR") {
      moveTo(x + 600, y + 600, light, "go BR " + ctx);
    } else {
      moveTo(x, 0, light, "go UP " + ctx);
    }
  }
  void moveToTarget(Creature &c, int light) {
    ostringstream oss;
    oss << "target " << c.id;
    move(c.direction.at(id), light, oss.str());
  }
};
typedef map<int, Drone> Drones;

/* ========================================================================= */
bool gotAllType(Creatures &creatures, int type) {
  int count = 0;
  int goal = 4;
  for (auto &p : creatures) {
    Creature &c = p.second;
    if (c.alive == false)
      goal--;
    if (c.scannedBy != -1 && c.type == type)
      count++;
  }
  return count >= goal;
}

void setAlives(Creatures &creatures, map<int, bool> alives) {
  cerr << "Dead creatures: ";
  for (auto &p : alives) {
    creatures.at(p.first).alive = p.second;
    if (p.second == false)
      cerr << p.first << " ";
  }
  cerr << endl;
}
/* ========================================================================= */

void parser(Creatures &creatures, Drones &myDrones, map<int, bool> &alives) {
  // ignore
  int myScore;
  cin >> myScore;
  cin.ignore();
  int foeScore;
  cin >> foeScore;
  cin.ignore();

  // get which creature I scanned
  int myScanCount;
  cin >> myScanCount;
  cin.ignore();
  for (int i = 0; i < myScanCount; i++) {
    int creatureId;
    cin >> creatureId;
    cin.ignore();
    creatures.at(creatureId).saved = true;
  }
  int foeScanCount;
  cin >> foeScanCount;
  cin.ignore();
  for (int i = 0; i < foeScanCount; i++) {
    int creatureId;
    cin >> creatureId;
    cin.ignore();
  }

  int myDroneCount;
  cin >> myDroneCount;
  cin.ignore();
  for (int i = 0; i < myDroneCount; i++) {
    int droneId;
    int droneX;
    int droneY;
    int emergency;
    int battery;
    cin >> droneId >> droneX >> droneY >> emergency >> battery;
    cin.ignore();
    myDrones.insert(
        make_pair(droneId, Drone(droneId, droneX, droneY, battery)));
  }
  int foeDroneCount;
  cin >> foeDroneCount;
  cin.ignore();
  vector<Drone> foeDrones;
  for (int i = 0; i < foeDroneCount; i++) {
    int droneId;
    int droneX;
    int droneY;
    int emergency;
    int battery;
    cin >> droneId >> droneX >> droneY >> emergency >> battery;
    cin.ignore();
    foeDrones.push_back(Drone(droneId, droneX, droneY, battery));
  }

  int droneScanCount;
  cin >> droneScanCount;
  cin.ignore();
  int totalScannedByMe = 0;
  for (int i = 0; i < droneScanCount; i++) {
    int droneId;
    int creatureId;
    cin >> droneId >> creatureId;
    cin.ignore();
    if (myDrones.find(droneId) != myDrones.end()) {
      creatures.at(creatureId).scannedBy = droneId;
    }
  }

  int visibleCreatureCount;
  cin >> visibleCreatureCount;
  cin.ignore();
  for (int i = 0; i < visibleCreatureCount; i++) {
    int creatureId;
    int creatureX;
    int creatureY;
    int creatureVx;
    int creatureVy;
    cin >> creatureId >> creatureX >> creatureY >> creatureVx >> creatureVy;
    cin.ignore();
    creatures.at(creatureId).x = creatureX;
    creatures.at(creatureId).y = creatureY;
  }

  int radarBlipCount;
  cin >> radarBlipCount;
  cin.ignore();
  for (int i = 0; i < radarBlipCount; i++) {
    int droneId;
    int creatureId;
    string radar;
    cin >> droneId >> creatureId >> radar;
    cin.ignore();
    myDrones.at(droneId).blip.at(radar).push_back(creatureId);
    creatures.at(creatureId).direction.at(creatureId) = radar;
    alives.at(creatureId) = true;
  }
  cerr << "DEBUG 2" << endl;
}

Creature *getTarget(Creatures &creatures, int otherTarget) {
  for (auto &p : creatures) {
    Creature &c = p.second;
    if (!c.isTargetable() || c.id == otherTarget) {
      continue;
    }
    return &c;
  }
  return (NULL);
}

void dispBlip(const Drone &drone) {
  for (auto &p : drone.blip) {
    const vector<int> &v = p.second;
    cerr << p.first << ": " << v.size() << ", ";
  }
  cerr << endl;
}
void routine(Creatures &creatures) {
  Drones myDrones;
  map<int, bool> alives;
  for (auto &p : creatures) {
    alives.insert(make_pair(p.first, false));
    Creature &c = p.second;
    for (Drones::iterator it = myDrones.begin(); it != myDrones.end(); it++) {
      c.direction.insert(make_pair(it->first, "none"));
    }
  }
  parser(creatures, myDrones, alives);
  setAlives(creatures, alives);

  Creature *lastTarget = NULL;
  for (auto &p : myDrones) {
    Drone &drone = p.second;
    cerr << "Drone: " << drone.id << endl;
    dispBlip(drone);
    if (drone.target == NULL || !drone.target->isTargetable())
      drone.target =
          getTarget(creatures, lastTarget != NULL ? lastTarget->id : -1);
    if (drone.target == NULL) {
      drone.move("UP", 0, "nothing to do");
    } else {
      drone.target = &creatures.at(drone.target->id);
      drone.moveToTarget(*drone.target, 0);
    }
    lastTarget = drone.target;
  }
}

int main() {
  int creaturesCount;
  cin >> creaturesCount;
  cin.ignore();
  Creatures creatures;
  for (int i = 0; i < creaturesCount; i++) {
    int id;
    int color;
    int type;
    cin >> id >> color >> type;
    cin.ignore();
    // creatures[id] = Creature(id, color, type);
    creatures.insert(make_pair(id, Creature(id, color, type)));
  }
  cerr << "Creatures count: " << creaturesCount << endl;

  while (1) {
    routine(creatures);
  }
}
