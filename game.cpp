#include <algorithm>
#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <math.h>
#include <pthread.h>
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

double getAngleFromBlip(string dir) {
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

  bool isTargatable() {
    return (_isAlive && _type != -1 && _isSaved == false && _scannedBy == -1);
  }
  bool isInZone(int zone) {
    return (_y >= F1 * (zone) && _y <= F1 * (zone + 1));
  }
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

  Point getStratPointFromZone(int zone) {
    Point ret;
    ret.x = _x > F2 ? F3 : F1;
    ret.y = (zone * F1) + MID;
    return ret;
  }

  int isDangerous(Point v, vector<Creature> monsters) {
    const int ratio = 32;
    array<Point, ratio + 1> droneSteps;
    Point p1 = getPos();
    Point p2 = {p1.x + v.x, p1.y + v.y};

    for (int i = 0; i <= ratio; i++) {
      droneSteps[i] = getPointOnSegmentRatio(p1, p2, i, ratio - i);
    }
    for (auto &monster : monsters) {
      for (int i = 0; i <= ratio; i++) {
        Point nexPos = monster.getNextPos();
        Point compare =
            getPointOnSegmentRatio(monster.getPos(), nexPos, i, ratio - i);
        if (::getDist(droneSteps[i], compare) < D_DANGER) {
          return monster._id;
        }
      }
    }
    if (p2.x > F4 - D_DANGER || p2.x < D_DANGER ||
        p2.y > F4 - D_DANGER)
      return -1;
    return 0;
  }

  bool isInTargetsZone(int targetsType) {
    return (_y >= F1 * (targetsType + 1) && _y <= F1 * (targetsType + 2));
  }

  void setSafeDirections(array<int, 360> &directions,
                         vector<int> &safeDirections,
                         vector<int> &mehDirections,
                         vector<Creature> monsters) {
    for (int angle = 0; angle < 360; angle++) {
      directions[angle] = isDangerous(getVector(angle), monsters);
      if (directions[angle] == 0)
        safeDirections.push_back(angle);
      else if (directions[angle] == -1)
        mehDirections.push_back(angle);
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
    if (v._type != -1) {
      v._x = 0;
      v._y = 0;
      v._Vx = 0;
      v._Vy = 0;
      v._scannedBy = -1;
    } else {
      v._x += v._Vx;
      v._y += v._Vy;
    }
  }
}
void parseVisibleCreatures(Creatures &m) {
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
    if (creature._type == -1 && (creature._x && creature._y)) {
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

void betterAngle(array<int, 360> directions, vector<int> safeDirections,
                 vector<int> mehDirections, int &directionAngle) {
  if (directions[directionAngle] == 0)
    return;
  cerr << "Danger! Pb id " << directions[directionAngle] << " on "
       << directionAngle << endl;
  cerr << safeDirections.size() << " alt safe directions" << endl;
    cerr << mehDirections.size() << " alt meh directions" << endl;
  if (safeDirections.empty() == false) {
    // sort one actual angle and possible angle delta
    sort(safeDirections.begin(), safeDirections.end(),
         [&directionAngle](const int a, const int b) {
           return (abs(directionAngle - a) < abs(directionAngle - b));
         });
    int i = 0;
    cerr << "safe >>> ";
    for (auto it = safeDirections.begin(); it != safeDirections.end() && i < 5;
         it++, i++) {
      cerr << *it << " ";
    }
    cerr << endl;
    cerr << "New angle direction: " << directionAngle << " -> ";
    directionAngle = safeDirections.front();
    cerr << directionAngle << endl;
  } else if (mehDirections.empty() == false) {
    sort(mehDirections.begin(), mehDirections.end(),
         [&directionAngle](const int a, const int b) {
           return (abs(directionAngle - a) < abs(directionAngle - b));
         });
    int i = 0;
    cerr << "meh >>> ";
    for (auto it = mehDirections.begin(); it != mehDirections.end() && i < 5;
         it++, i++) {
      cerr << *it << " ";
    }
    cerr << endl;
    cerr << "New angle direction: " << directionAngle << " -> ";
    directionAngle = mehDirections.front();
    cerr << directionAngle << endl;
  }
}

void routine(Creatures &allCreatures, Drones &myDrones) {
  static int routineCount = 0;
  vector<Creature> monsters{};
  array<vector<Creature>, 3> targetableByType{};
  array<vector<Creature>, 3> scannedByType{};
  vector<Creature> scannedBys{};

  // Fill vectors
  for (auto &[creatureId, creature] : allCreatures) {
    if (creature._type == -1 && creature._Vx && creature._Vy) {
      monsters.push_back(creature);
    } else if (creature.isTargatable()) {
      targetableByType[creature._type].push_back(creature);
    } else if (creature._scannedBy != -1) {
      scannedByType[creature._type].push_back(creature);
      scannedBys.push_back(creature);
    }
  }

  int mustTargetType = -1;

  // Be sure to comple a zone before going the otherone
  if (targetableByType[2].size()) {
    mustTargetType = 2;
  } else if (targetableByType[1].size() && scannedByType[2].empty()) {
    mustTargetType = 1;
  } else if (targetableByType[0].size() && scannedByType[1].empty()) {
    mustTargetType = 0;
  }
  cerr << "Must target type: " << mustTargetType << endl;

  int droneLoopCount = 0;
  for (auto &[droneId, drone] : myDrones) {
    Drone otherDrone = myDrones.at((droneId + 2) % 4);
    cerr << ">>> Drone: " << drone._id << endl;
    int directionAngle = 90;
    Creature *target = NULL;
    ostringstream ctx;

    // get possible score in scanned and go save depend on it
    int scannedCount = 0;
    for (auto &c : scannedBys) {
      if (c._scannedBy == drone._id)
        scannedCount++;
    }

    if (mustTargetType != -1) {
      if (drone.isInTargetsZone(mustTargetType)) {
        ctx << " IN ZONE";
        if (!droneLoopCount) {
          for (auto it = targetableByType[mustTargetType].rbegin();
               it != targetableByType[mustTargetType].rend(); it++) {
            target = &(*it);
            ctx << " HUNTING";
            break;
          }
        } else {
          for (auto &creature : targetableByType[mustTargetType]) {
            target = &creature;
            ctx << " HUNTING";
            break;
          }
        }
      } else if (((mustTargetType + 1) * F1) > drone._y) {
        ctx << " MOVING";
        Point next = drone.getStratPointFromZone(mustTargetType + 1);
        directionAngle = getAngle({next.x - drone._x, next.y - drone._y});
      }
    }
    if (target) {
      Point next = drone.getStratPointFromZone(mustTargetType + 1);
      if (target->_type == mustTargetType) {
        if (drone.blip.at(target->_id).back() == 'R') {
          next.x = drone._x + 600;
        } else {
          next.x = drone._x - 600;
        }
      }
      directionAngle = getAngle({next.x - drone._x, next.y - drone._y});
      cerr << "target: " << target->_id << endl;
    }
    ctx << " " << directionAngle;
    int light = 0;
    if (drone._y >= F1 && routineCount % 2 == 0) {
      light = 1;
    }

    array<int, 360> directions{};
    vector<int> safeDirections{};
    vector<int> mehDirections{};
    drone.setSafeDirections(directions, safeDirections, mehDirections,
                            monsters);

    betterAngle(directions, safeDirections, mehDirections, directionAngle);

    Point v = getVector(directionAngle);
    moveTo(drone._x + v.x, drone._y + v.y, light, ctx.str());
    droneLoopCount++;
  }
  routineCount++;
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
