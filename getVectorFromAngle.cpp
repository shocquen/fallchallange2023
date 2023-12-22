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
int main(int argc, char **argv) {
  if (argc != 2)
    return 1;
  double angle = stod(argv[1]);

  Point ret = getVector(angle);

  cout << ret.first << " " << ret.second << endl;
  return 0;
}
