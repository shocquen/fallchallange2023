#include <cmath>
#include <iostream>
#include <string>
#include <utility>
using namespace std;
typedef std::pair<double, double> Point;
double getDist(Point p1, Point p2) {
  return (sqrt(pow(p2.first - p1.first, 2) + pow(p2.second - p1.second, 2)));
}

int main(int argc, char **argv) {
  if (argc == 5)
    cout << getDist({stod(argv[1]) ,stod(argv[2])}, {stod(argv[3]) ,stod(argv[4])});
  return 0;
}
