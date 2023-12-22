#include <algorithm>
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
#define D_DANGER 500
#define ONE_EIGHTY_PI 57.2957795131
#define PI_ONE_EIGHTY 0.01745329251

using namespace std;

enum Floors {
  MID = 2500 / 2,
  F0 = 0,
  F1 = 2500,
  F2 = 5000,
  F3 = 7500,
  F4 = 10000,
};

typedef pair<double, double> Point;

double getDist(Point p1, Point p2) {
  return (sqrt(pow(p2.first - p1.first, 2) + pow(p2.second - p1.second, 2)));
}
double getMagnetude(Point vector) {
  return (sqrt(pow(vector.first, 2) + pow(vector.second, 2)));
}
Point getVector(double angle) {
  return {round(cos(angle * PI_ONE_EIGHTY) * D_MAX),
          round(sin(angle * PI_ONE_EIGHTY) * -D_MAX)};
}
bool onSegment(Point p, Point q, Point r) {
  if (q.first <= max(p.first, r.first) && q.first >= min(p.first, r.first) &&
      q.second <= max(p.second, r.second) &&
      q.second >= min(p.second, r.second))
    return true;

  return false;
}
int orientation(Point p, Point q, Point r) {
  int val = (q.second - p.second) * (r.first - q.first) -
            (q.first - p.first) * (r.second - q.second);
  if (val == 0)
    return 0;               // collinear
  return (val > 0) ? 1 : 2; // clock or counterclock wise
}
bool doIntersect(Point p1, Point q1, Point p2, Point q2) {
  int o1 = orientation(p1, q1, p2);
  int o2 = orientation(p1, q1, q2);
  int o3 = orientation(p2, q2, p1);
  int o4 = orientation(p2, q2, q1);

  if (o1 != o2 && o3 != o4)
    return true;
  if (o1 == 0 && onSegment(p1, p2, q1))
    return true;
  if (o2 == 0 && onSegment(p1, q2, q1))
    return true;
  if (o3 == 0 && onSegment(p2, p1, q2))
    return true;
  if (o4 == 0 && onSegment(p2, q1, q2))
    return true;
  return false;
}

double getAngle(Point vector) {
  double x = atan(abs(vector.second / vector.first)) * ONE_EIGHTY_PI;

  if (vector.first > 0 && vector.second < 0)
    return x;
  else if (vector.first < 0 && vector.second < 0)
    return 180 - x;
  else if (vector.first < 0 && vector.second > 0)
    return 180 + x;
  return 360 - x;
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

  Point getNextPos() { return {_x + _Vx, _y + _Vy}; }
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
  double getDist(Creature c) { return (::getDist({_x, _y}, {c._x, c._y})); }
  double getNextDist(Creature c, Point next) {
    Point nextCreaturePos = c.getNextPos();
    return (
        ::getDist({_x, _y}, {nextCreaturePos.first, nextCreaturePos.second}));
  }
  bool isDangerous(Creature c, Point next) {
    if (c._type != -1)
      return false;
    if (doIntersect({_x, _y}, next, {c._x, c._y}, c.getNextPos()))
      return true;
    if (getNextDist(c, next) <= D_DANGER)
      return true;
    return false;
  }
};
typedef map<int, Drone> Drones;

/* ========================================================================= */

void creaturesParser(Creatures &m) {
  int creatureCount;
  cin >> creatureCount;
  cin.ignore();
  for (int i = 0; i < creatureCount; i++) {
    int id;
    int color;
    int type;
    cin >> id >> color >> type;
    cin.ignore();
    m.insert({id, Creature(id, color, type)});
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

void parseDroneScannes(Creatures &m, Drones d) {
  int droneScanCount;
  cin >> droneScanCount;
  cin.ignore();
  for (int i = 0; i < droneScanCount; i++) {
    int droneId;
    int creatureId;
    cin >> droneId >> creatureId;
    cin.ignore();
    if (d.find(droneId) != d.end())
      m.at(creatureId)._scannedBy = droneId;
  }
}

void parseVisibleCreatures(Creatures &m) {
  int visibleCreatureCount;
  cin >> visibleCreatureCount;
  cin.ignore();
  for (auto &p : m) {
    Creature &c = p.second;
    c._x = 0;
    c._y = 0;
    c._Vx = 0;
    c._Vy = 0;
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
  for (auto &p : allCreatures) {
    Creature &c = p.second;
    c._isAlive = false;
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
  parseDroneScannes(allCreatures, myDrones);
  parseVisibleCreatures(allCreatures);
  parseBlip(myDrones, allCreatures);
}

void debugBlip(Drone d) {
  cerr << setw(6) << "TL: ";
  for (auto &p1 : d.blip) {
    if (p1.second == "TL")
      cerr << p1.first << " ";
  }
  cerr << endl;
  cerr << setw(6) << "TR: ";
  for (auto &p1 : d.blip) {
    if (p1.second == "TR")
      cerr << p1.first << " ";
  }
  cerr << endl;
  cerr << setw(6) << "BL: ";
  for (auto &p1 : d.blip) {
    if (p1.second == "BL")
      cerr << p1.first << " ";
  }
  cerr << endl;
  cerr << setw(6) << "BR: ";
  for (auto &p1 : d.blip) {
    if (p1.second == "BR")
      cerr << p1.first << " ";
  }
  cerr << endl;
}
void debug(Creatures allCreatures, Drones myDrones) {
  for (auto &p : myDrones) {
    Drone d = p.second;
    cerr << "Drone " << d._id << ": ";
    cerr << d._x << ", " << d._y << endl;

    cerr << setw(2) << ""
         << "scanneds: ";
    for (auto &p : allCreatures) {
      Creature c = p.second;
      if (c._scannedBy == d._id)
        cerr << c._id << " ";
    }
    cerr << endl;
  }

  cerr << "Dead creatures: ";
  for (auto &p : allCreatures) {
    if (p.second._isAlive == false)
      cerr << p.second._id << " ";
  }
  cerr << endl;

  cerr << "Monsters: " << endl;
  for (auto &p : allCreatures) {
    Creature c = p.second;
    if (c._type == -1 && (c._Vx && c._Vy)) {
      cerr << setw(4) << c._id << " [" << c._x << ", " << c._y << "]->[";
      Point nextP = c.getNextPos();
      cerr << nextP.first << ", " << nextP.second << "] " << c.getDirection()
           << endl;
    }
  }
  cerr << endl;
}

/* ========================================================================= */
void moveTo(int x, int y, int light, string ctx) {
  cout << "MOVE " << x << " " << y << " " << light << " " << ctx << endl;
}

int main() {
  Creatures allCreatures;
  Drones myDrones;

  creaturesParser(allCreatures);
  Creature *target;
  while (1) {
    parse(allCreatures, myDrones);
    // debug(allCreatures, myDrones);

    int i = 0;
    for (auto &dP : myDrones) {
      Drone &d = dP.second;
      // cout << "WAIT 1 i: " << i << endl;

      for (auto &dC : allCreatures) {
        if (dC.second._type != -1 && dC.second._isAlive &&
            dC.second._isSaved == false && dC.second._scannedBy == -1) {
          target = &dC.second;
          break;
        }
      }
      cerr << "Target: " << target->_id << " ";
      if (target) {
        string dir = d.blip.at(target->_id);
        cerr << dir << endl;
        Point v = getVector(90);
        if (dir == "TL") {
          v = getVector(135);
        } else if (dir == "TR") {
          v = getVector(45);
        } else if (dir == "BL") {
          v = getVector(225);
        } else if (dir == "BR") {
          v = getVector(315);
        }
        moveTo(d._x + v.first, d._y + v.second, 1, dir);
      }
      i++;
    }
  }
}
