
#include <algorithm>
#include <array>
#include <iostream>
#include <ostream>
#include <cmath>
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

double getDist(Point p1, Point p2) {
  return (sqrt(pow(p2.x - p1.x, 2) + pow(p2.x - p1.x, 2)));
}
// ex: get C = 2/3 of AB then m = 2 and n = 3 - m = 1;
Point getPointOnSegmentRatio(Point p1, Point p2, int m, int n) {
  Point ret{};
  const int r = m + n;
  ret.x = ((m * p2.x) + (n * p1.x)) / r;
  ret.y = ((m * p2.y) + (n * p1.y)) / r;
  return ret;
}

void foo(Point p1, Point p2, Point p3, Point p4) {
  const int ratio = 6;
  std::array<Point, ratio - 1> seg1;
  std::array<Point, ratio - 1> seg2;


  for (int i = 0; i < ratio - 1 ; i++) {
    seg1[i] = getPointOnSegmentRatio(p1, p2, i + 1, ratio - (i + 1));
    seg2[i] = getPointOnSegmentRatio(p3, p4, i + 1, ratio - (i + 1));
    std::cout << "seg1 " << i << ": " << seg1[i].x << " " << seg1[i].y << "\n";
    std::cout << "seg2 " << i << ": " << seg2[i].x << " " << seg2[i].y << "\n";
    std::cout << "dist: " << getDist(seg1[i], seg2[i]) << '\n';
  }

}

int main(void) {
	Point p1 = {5, 5};
	Point p2 = {-1, -4};
	Point p3 = {3, 5};
	Point p4 = {-3, -4};

	// Point p3 = getPointOnSegmentRatio(p1, p2, 1, 1);
	// std::cout << p3.x << ", " << p3.y << std::endl;

  foo(p1, p2, p3, p4);
}
