#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

enum Levels {
  MID_STEP = 2500 / 2,
  LEVEL0 = 0,
  LEVEL1 = 2500,
  LEVEL2 = 5000,
  LEVEL3 = 7500,
  LEVEL4 = 10000,
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
  Creature(int id, int color, int type) {
    this->id = id;
    this->color = color;
    this->type = type;
    x = 0;
    y = 0;
    saved = false;
    scannedBy = -1;
  }
  Creature(const Creature &copy) { *this = copy; }
  int id, color, type;
  int x;
  int y;
  string direction;
  int scannedBy;
  bool saved;

  Creature &operator=(const Creature &rhs) {
    id = rhs.id;
    color = rhs.color;
    type = rhs.type;
    x = rhs.x;
    y = rhs.y;
    saved = rhs.saved;
    scannedBy = rhs.scannedBy;
    return *this;
  }
  bool operator==(const Creature &rhs) { return id = rhs.id; }
  bool operator==(const int id) { return this->id = id; }
  bool isTargetable() { return !(saved || scannedBy != -1); }
};

class Drone {
public:
  Drone(int id, int x, int y, int battery)
      : id(id), x(x), y(y), battery(battery) {}
  int id;
  int x, y;
  int battery;
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
      moveTo(x, 0, light, "go UP");
    }
  }
  void moveToTarget(Creature &c, int light) {
    ostringstream oss;
    oss << "target " << c.id;
    move(c.direction, light, oss.str());
  }
};

/* ========================================================================= */
typedef map<int, Creature> Creatures;
typedef map<int, Drone> Drones;
void parser(Creatures &creatures, Drones &myDrones) {
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
  for (int i = 0; i < droneScanCount; i++) {
    int droneId;
    int creatureId;
    cin >> droneId >> creatureId;
    cin.ignore();
    if (myDrones.find(droneId) != myDrones.end())
      creatures.at(creatureId).scannedBy = droneId;
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
    creatures.at(creatureId).direction = radar;
  }
}

void routine(Creatures &creatures) {
  static bool done = false;
  Drones myDrones;
  parser(creatures, myDrones);

  int droneN = 0;
  for (auto &p : myDrones) {
    Drone &drone = p.second;

    size_t last = 0;
    cerr << "Drone: " << p.first << endl;
    for (auto &pBlip : drone.blip) {
      vector<int> &v = pBlip.second;
      cerr << pBlip.first << ": " << v.size() << ", ";
    }
    cerr << endl;

    int light = 0;
    int nextX, nextY;
    nextY = 10000;
    if (droneN == 0)
      nextX = 2500;
    else
      nextX = 7500;

    if ((drone.y > (LEVEL1 + MID_STEP) - 350 && drone.y < (LEVEL1 + MID_STEP) + 350) ||
        (drone.y > (LEVEL2 + MID_STEP) - 350 && drone.y < (LEVEL2 + MID_STEP) + 350) ||
        (drone.y > (LEVEL3 + MID_STEP) - 350 && drone.y < (LEVEL3 + MID_STEP) + 350)) {
      light = 1;
    }
    if (drone.y >= LEVEL3 + MID_STEP + 350) {
      done = true;
    }
    if (done)
      nextY = LEVEL0;
    moveTo(nextX, nextY, light, "GO!");
    droneN++;
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
