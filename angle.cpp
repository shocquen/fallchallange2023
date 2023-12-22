#include <cmath>
#include <iostream>
#include <string>
#include <utility>
using namespace std;
typedef std::pair<double, double> Point;

double getAngle(Point A, Point B) {
  Point v = {B.first - A.first, B.second - A.second};
  double x = atan(abs(v.second / v.first)) * (180 / M_PI);

  cout << "v = " << v.first << " " << v.second << endl;
  cout << "Q1 = " << x << endl;
  cout << "Q2 = " << 180 - x << endl;
  cout << "Q3 = " << 180 + x << endl;
  cout << "Q4 = " << 360 - x << endl;
  if (v.first < 0 && v.second < 0)
    return 180 - x;
  else if (v.first < 0 && v.second > 0)
    return 180 + x;
  else if (v.first > 0 && v.second > 0)
    return 360 - x;
  return x;
}

double getAngle(Point vector) {
  double x = atan(abs(vector.second / vector.first)) * (180 / M_PI);

  cout << "vector = " << vector.first << " " << vector.second << endl;
  cout << "Q1 = " << x << endl;
  cout << "Q2 = " << 180 - x << endl;
  cout << "Q3 = " << 180 + x << endl;
  cout << "Q4 = " << 360 - x << endl;
  if (vector.first > 0 && vector.second < 0)
    return x;
  else if (vector.first < 0 && vector.second < 0)
    return 180 - x;
  else if (vector.first < 0 && vector.second > 0)
    return 180 + x;
  return 360 - x;
}

int main(int argc, char **argv) {
  double angle;
  if (argc > 3) {
    Point A = {stod(argv[1]), stod(argv[2])};
    Point B = {stod(argv[3]), stod(argv[4])};

    angle = getAngle(A, B);
  } else {
    angle = getAngle({stod(argv[1]), stod(argv[2])});
  }
  cout << "=================================" << endl;
  cout << "angle = " << angle << endl;

  return 0;
}
