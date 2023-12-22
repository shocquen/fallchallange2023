#include <cmath>
#include <iostream>
#include <string>
#include <utility>

#define D_MAX 600

using namespace std;
typedef std::pair<double, double> Point;

Point getVector(double angle) {
  return {round(cos(angle * (M_PI / 180)) * D_MAX),
          round(sin(angle * (M_PI / 180)) * -D_MAX)};
};
Point getVector(double angle, double mag) {
  return {round(cos(angle * (M_PI / 180)) * mag),
          round(sin(angle * (M_PI / 180)) * -mag)};
};
int main(int argc, char **argv) {
  if (argc == 2) {
    double angle = stod(argv[1]);
    Point ret = getVector(angle);
    cout << ret.first << " " << ret.second << endl;
  } else if (argc == 3) {
    double angle = stod(argv[1]);
    double mag = stod(argv[2]);
    Point ret = getVector(angle, mag);
    cout << ret.first << " " << ret.second << endl;
  }
  return 0;
}
