#include <algorithm>
#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <math.h>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#define D_MAX 600
#define D_DANGER 620
#define ONE_EIGHTY_PI 57.2957795131
#define PI_ONE_EIGHTY 0.01745329251

using namespace std;

enum Floors {
  MID = 2500 / 2,
  F0 = 0,
  F1 = 2500,
  F1MID = F1 + MID,
  F2 = 5000,
  F2MID = F2 + MID,
  F3 = 7500,
  F3MID = F3 + MID,
  F4 = 10000,
};

// typedef pair<double, double> Point;
typedef struct Point {
  double x;
  double y;
  Point() {
    this->x = 0;
    this->y = 0;
  }
  Point(double x, double y) {
    this->x = x;
    this->y = y;
  };
  Point(int x, int y) {
    this->x = x;
    this->y = y;
  };
} Point;

// ex: get C = 2/3 of AB then m = 2 and n = 3 - m = 1;
Point getPointOnSegmentRatio(Point p1, Point p2, int m, int n) {
  Point ret{};
  const int r = m + n;
  ret.x = ((m * p2.x) + (n * p1.x)) / r;
  ret.y = ((m * p2.y) + (n * p1.y)) / r;
  return ret;
}

double getDist(Point p1, Point p2) {
  return (sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2)));
}
double getMagnetude(Point vector) {
  return (sqrt(pow(vector.x, 2) + pow(vector.y, 2)));
}
Point getVector(double angle) {
  return {round(cos(angle * PI_ONE_EIGHTY) * D_MAX),
          round(sin(angle * PI_ONE_EIGHTY) * -D_MAX)};
}

double getAngle(Point vector) {
  double a = atan(abs(vector.y / vector.x)) * ONE_EIGHTY_PI;

  if (vector.x > 0 && vector.y < 0)
    return a;
  else if (vector.x < 0 && vector.y < 0)
    return 180 - a;
  else if (vector.x < 0 && vector.y > 0)
    return 180 + a;
  return 360 - a;
}

/* ========================================================================= */

class Creature {
public:
  int _id, _color, _type, _x, _y, _Vx, _Vy;
  int _scannedBy;
  bool _onMap;
  bool _isAlive;
  bool _isSaved;
  Creature(int id, int color, int type) {
    _id = id;
    _color = color;
    _isAlive = false;
    _onMap = false;
    _scannedBy = -1;
    _type = type;
    _isSaved = false;
    _x = 0;
    _y = 0;
    _Vx = 0;
    _Vy = 0;
  }
  Creature(const Creature &copy) { *this = copy; }
  Creature &operator=(const Creature &rhs) {
    _id = rhs._id;
    _color = rhs._color;
    _isAlive = rhs._isAlive;
    _onMap = rhs._onMap;
    _scannedBy = rhs._scannedBy;
    _type = rhs._type;
    _isSaved = rhs._isSaved;
    _x = rhs._x;
    _y = rhs._y;
    _Vx = rhs._Vx;
    _Vy = rhs._Vy;
    return *this;
  }

  bool isTargatable() { return (_isAlive && _type != -1 && _scannedBy == -1); }
  Point getNextPos() { return {_x + _Vx, _y + _Vy}; }
  Point getPos() { return {_x, _y}; }
  double getDirection() { return (getAngle({_Vx, _Vy})); }
};
typedef map<int, Creature> Creatures;

class Drone {
public:
  int _id, _x, _y, _battery;
  map<int, string> blip;
  Drone(int id, int x, int y, int battery) {
    _id = id;
    _x = x;
    _y = y;
    _battery = battery;
  }

  Point getPos() { return {_x, _y}; }

  int isDangerous(Point v, vector<Creature> monsters) {
    const int ratio = 32;
    Point droneSteps[ratio]{};
    Point p1 = getPos();
    Point p2 = {p1.x + v.x, p1.y + v.y};

    for (int i = 0; i < ratio - 1; i++) {
      droneSteps[i] = getPointOnSegmentRatio(p1, p2, i + 1, ratio - (i + 1));
    }
    for (auto &monster : monsters) {
      for (int i = 0; i < ratio - 1; i++) {
        Point nexPos = monster.getNextPos();
        Point compare = getPointOnSegmentRatio(monster.getPos(), nexPos, i + 1,
                                               ratio - (i + 1));
        if (::getDist(droneSteps[i], compare) < D_DANGER) {
          return monster._id;
        }
      }
    }
    return 0;
  }

  void setSafeDirections(array<int, 360> &directions,
                         vector<Creature> monsters) {
    for (int angle = 0; angle < 360; angle++) {
      directions[angle] = isDangerous(getVector(angle + 1), monsters);
    }
  }
};
typedef map<int, Drone> Drones;

/* ========================================================================= */

void creaturesParser(Creatures &creatures) {
  int creatureCount;
  cin >> creatureCount;
  cin.ignore();
  for (int i = 0; i < creatureCount; i++) {
    int id;
    int color;
    int type;
    cin >> id >> color >> type;
    cin.ignore();
    auto c = creatures.insert({id, Creature(id, color, type)});
  }
  cerr << "Creatures count: " << creatureCount << endl;
}

/* ------------------------------------------------------------------------- */

void parseScore() {
  int myScore;
  cin >> myScore;
  cin.ignore();
  int foeScore;
  cin >> foeScore;
  cin.ignore();
}

void parseSavedCreatures(Creatures &m) {
  int myScanCount;
  cin >> myScanCount;
  cin.ignore();
  for (int i = 0; i < myScanCount; i++) {
    int creatureId;
    cin >> creatureId;
    cin.ignore();
    m.at(creatureId)._isSaved = true;
  }
  int foeScanCount;
  cin >> foeScanCount;
  cin.ignore();
  for (int i = 0; i < foeScanCount; i++) {
    int creatureId;
    cin >> creatureId;
    cin.ignore();
  }
}

void parseDrones(Drones &m) {
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
    Drones::iterator it = m.find(droneId);
    if (it != m.end()) {
      Drone &d = it->second;
      d._battery = battery;
      d._x = droneX;
      d._y = droneY;
    } else {
      m.insert({droneId, Drone(droneId, droneX, droneY, battery)});
    }
  }
  int foeDroneCount;
  cin >> foeDroneCount;
  cin.ignore();
  for (int i = 0; i < foeDroneCount; i++) {
    int droneId;
    int droneX;
    int droneY;
    int emergency;
    int battery;
    cin >> droneId >> droneX >> droneY >> emergency >> battery;
    cin.ignore();
  }
}

void parseDroneScannes(Creatures &creatures, Drones drone) {
  int droneScanCount;
  cin >> droneScanCount;
  cin.ignore();
  for (int i = 0; i < droneScanCount; i++) {
    int droneId;
    int creatureId;
    cin >> droneId >> creatureId;
    cin.ignore();
    if (drone.find(droneId) != drone.end())
      creatures.at(creatureId)._scannedBy = droneId;
  }
}

void resetCreatures(Creatures &creatures) {
  for (auto &[k, v] : creatures) {
    v._x = 0;
    v._y = 0;
    v._Vx = 0;
    v._Vy = 0;
    v._scannedBy = -1;
  }
}
void parseVisibleCreatures(Creatures &m) {
  int visibleCreatureCount;
  cin >> visibleCreatureCount;
  cin.ignore();
  for (auto &[k, v] : m) {
    v._x = 0;
    v._y = 0;
    v._Vx = 0;
    v._Vy = 0;
  }
  for (int i = 0; i < visibleCreatureCount; i++) {
    int creatureId;
    int creatureX;
    int creatureY;
    int creatureVx;
    int creatureVy;
    cin >> creatureId >> creatureX >> creatureY >> creatureVx >> creatureVy;
    cin.ignore();
    Creature &c = m.at(creatureId);
    c._x = creatureX;
    c._y = creatureY;
    c._Vx = creatureVx;
    c._Vy = creatureVy;
  }
}

void parseBlip(Drones &m, Creatures &allCreatures) {
  int radarBlipCount;
  cin >> radarBlipCount;
  cin.ignore();
  for (auto &[k, v] : allCreatures) {
    v._isAlive = false;
  }
  for (int i = 0; i < radarBlipCount; i++) {
    int droneId;
    int creatureId;
    string radar;
    cin >> droneId >> creatureId >> radar;
    cin.ignore();

    allCreatures.at(creatureId)._isAlive = true;
    map<int, string> &d = m.at(droneId).blip;
    if (d.find(creatureId) != d.end()) {
      d.at(creatureId) = radar;
    } else {
      d.insert({creatureId, radar});
    }
  }
}

/* ------------------------------------------------------------------------- */

void parse(Creatures &allCreatures, Drones &myDrones) {
  parseScore();
  parseSavedCreatures(allCreatures);
  parseDrones(myDrones);
  resetCreatures(allCreatures);
  parseDroneScannes(allCreatures, myDrones);
  parseVisibleCreatures(allCreatures);
  parseBlip(myDrones, allCreatures);
}

void debugDroneBlip(Drone d) {
  cerr << setw(6) << "TL: ";
  for (auto &p1 : d.blip) {
    if (p1.second == "TL")
      cerr << p1.second << " ";
  }
  cerr << endl;
  cerr << setw(6) << "TR: ";
  for (auto &p1 : d.blip) {
    if (p1.second == "TR")
      cerr << p1.second << " ";
  }
  cerr << endl;
  cerr << setw(6) << "BL: ";
  for (auto &p1 : d.blip) {
    if (p1.second == "BL")
      cerr << p1.second << " ";
  }
  cerr << endl;
  cerr << setw(6) << "BR: ";
  for (auto &p1 : d.blip) {
    if (p1.second == "BR")
      cerr << p1.second << " ";
  }
  cerr << endl;
}
void debugDrones(Drones myDrones, Creatures allCreatures) {
  for (auto &[k, v] : myDrones) {
    cerr << "Drone " << v._id << ": ";
    cerr << v._x << ", " << v._y << endl;

    cerr << setw(2) << ""
         << "scanneds: ";
    for (auto &p : allCreatures) {
      Creature c = p.second;
      if (c._scannedBy == v._id)
        cerr << c._id << " ";
    }
    cerr << endl;
    debugDroneBlip(v);
  }
}
void debugDeadCreatures(Creatures allCreatures) {
  cerr << "Dead creatures: ";
  for (auto &p : allCreatures) {
    if (p.second._isAlive == false)
      cerr << p.second._id << " ";
  }
  cerr << endl;
}
void debugMonsters(Creatures allCreatures, Drones myDrones) {
  cerr << "Monsters: " << endl;
  for (auto &[creatureId, creature] : allCreatures) {
    if (creature._type == -1 && (creature._Vx && creature._Vy)) {
      cerr << setw(4) << creature._id << " [" << creature._x << ", "
           << creature._y << "]->[";
      Point nextP = creature.getNextPos();
      cerr << nextP.x << ", " << nextP.y << "] " << creature.getDirection()
           << " | ";

      for (auto &[droneId, drone] : myDrones) {
        cerr << droneId << " -> " << getDist(drone.getPos(), creature.getPos())
             << " | ";
      }
      cerr << endl;
    }
  }
  cerr << endl;
}
void debug(Creatures allCreatures, Drones myDrones) {
  // debugDrones(myDrones, allCreatures);
  // debugDeadCreatures(allCreatures);
  debugMonsters(allCreatures, myDrones);
}

/* ========================================================================= */
void moveTo(int x, int y, int light, string ctx) {
  cout << "MOVE " << x << " " << y << " " << light << " " << ctx << endl;
}

Point vectorFromDir(string dir) {
  if (dir == "TL") {
    return {-424, -424};
  } else if (dir == "TR") {
    return {424, -424};
  } else if (dir == "BL") {
    return {-424, 424};
  } else if (dir == "BR") {
    return {424, 424};
  }
  return {0, -600};
}
double angleFromDir(string dir) {
  if (dir == "TL") {
    return 135;
  } else if (dir == "TR") {
    return 45;
  } else if (dir == "BL") {
    return 225;
  } else if (dir == "BR") {
    return 315;
  }
  return 90;
}

void routine(Creatures &allCreatures, Drones &myDrones) {

  vector<Creature> monsters;
  array<int, 3> countByType{};

  for (auto &[creatureId, creature] : allCreatures) {
    if (creature._type == -1 && creature._Vx && creature._Vy) {
      monsters.push_back(creature);
    } else if (creature.isTargatable()) {
      countByType[creature._type]++;
    }
  }

  int mustTargetType = -1;
  for (int i = 2; i >= 0; i--) {
    if (countByType[i]) {
      mustTargetType = i;
      break;
    }
  }
  cerr << "Must target type: " << mustTargetType << endl;

  int droneLoopCount = 0;
  for (auto &[droneId, drone] : myDrones) {
    cerr << ">>> Drone: " << drone._id << endl;
    Point next = {F1, F0};
    if (droneLoopCount) {
      next.x = F3;
    }
    switch (mustTargetType) {
    case 0:
      next.y = F1MID;
      break;
    case 1:
      next.y = F2MID;
      break;
    case 2:
      next.y = F3MID;
      break;
    }
    cerr << "next: " << next.x << ", " << next.y << endl;

    int directionAngle = getAngle({next.x - drone._x, next.y - drone._y});
    ostringstream ctx;
    ctx << " " << directionAngle;
    int light = 0;
    if ((drone._y >= F1 + 1000 && drone._y <= F2 - 1000) ||
        (drone._y >= F2 + 1000 && drone._y <= F3 - 1000) ||
        (drone._y >= F3 + 1000 && drone._y <= F4 - 1000)) {
      light = 1;
    }

    array<int, 360> directions{};
    drone.setSafeDirections(directions, monsters);
    Point v = getVector(directionAngle);

    // if danger on the way reroute
    if (directions[directionAngle - 1]) {
      cerr << "Danger, there is monster " << directions[directionAngle - 1]
           << " on " << directionAngle << endl;
      light = 0;
      for (int i = 1; i <= 360; i++) {
        int plus, minus;
        plus = (directionAngle + i) % 360;
        minus = abs(directionAngle - i) % 360;
        if (directions[plus] == 0) {
          v = getVector(plus);
          cerr << "New direction: " << plus << endl;
          break;
        }
        if (directions[minus] == 0) {
          v = getVector(minus);
          cerr << "New direction: " << minus << endl;
          break;
        }
      }
    }

    moveTo(drone._x + v.x, drone._y + v.y, light, ctx.str());
    droneLoopCount++;
  }
}
int main() {
  Creatures allCreatures;
  Drones myDrones;

  creaturesParser(allCreatures);
  while (1) {
    parse(allCreatures, myDrones);
    debug(allCreatures, myDrones);
    routine(allCreatures, myDrones);
  }
}
