#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <math.h>
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

typedef pair<double, double> Point;

double getAngle(Point A, Point B) {
    Point v = {B.first - A.first, B.second - A.second};
    double x = atan(abs(v.second / v.first)) * (180 / M_PI);

	if (v.first < 0 && v.second > 0)
        return 180 - x;
	else if (v.first < 0 && v.second < 0)
        return 180 + x;
	else if (v.first > 0 && v.second < 0)
      return 360 - x;
    return x;
}
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

  Point getNextPos() {
    return {_x + _Vx, _y + _Vy};
  }
  double getDirection() {
    return (getAngle({_x, _y}, getNextPos()));
  }
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
  Point getNextPos() {
    return {};
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

void debug(Creatures allCreatures, Drones myDrones) {
  for (auto &p : myDrones) {
    Drone d = p.second;
    cerr << "Drone " << d._id << ": ";
    cerr << d._x << ", " << d._y << endl;
    cerr << setw(5) << "TL: ";
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
    cerr << setw(2) << "" << "scanneds: ";
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
    if (c._type == -1) {
      cerr << setw(4) <<  c._id << " [" << c._x << ", " << c._y << "]->[";
      Point nextP = c.getNextPos();
      cerr << nextP.first << ", " << nextP.second << "] " << c.getDirection() << endl;
    }
  }
  cerr << endl;
}

/* ========================================================================= */

int main() {
  Creatures allCreatures;
  Drones myDrones;

  creaturesParser(allCreatures);
  while (1) {
    parse(allCreatures, myDrones);
    debug(allCreatures, myDrones);

    cout << "WAIT 1" << endl; // MOVE <x> <y> <light (1|0)> | WAIT <light (1|0)>
    cout << "WAIT 1" << endl; // MOVE <x> <y> <light (1|0)> | WAIT <light (1|0)>
  }
}
