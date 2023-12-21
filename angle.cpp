#include <cmath>
#include <utility>
#include <iostream>
#include <string>
using namespace std;
typedef std::pair<double, double> Point;
int main(int argc, char **argv) {
	Point A = {stod(argv[1])/10000, stod(argv[2])/10000};
	Point B = {stod(argv[3])/10000, stod(argv[4])/10000};

	cout << "A = (" << A.first << ", " << A.second << ")" << endl;
	cout << "B = (" << B.first << ", " << B.second << ")" << endl;
	Point v = {B.first - A.first, B.second - A.second};
	cout << "v = (" << v.first << ", " << v.second << ")" << endl;
	double angle = atan(abs(v.second / v.first)) * (180 / M_PI);

	if (v.first < 0 && v.second > 0)
		cout << "angle Q2 = " << 180 - angle << endl;
	else if (v.first < 0 && v.second < 0)
		cout << "angle Q3 = " << 180 + angle << endl;
	else if (v.first > 0 && v.second < 0)
		cout << "angle Q4 = " << 360 - angle << endl;
	else
		cout << "angle Q1 = " << angle << endl;

cout << "=================================" << endl;
	cout << "angle Q2 = " << 180 - angle << endl;
	cout << "angle Q3 = " << 180 + angle << endl;
	cout << "angle Q4 = " << 360 - angle << endl;
	cout << "angle Q1 = " << angle << endl;

	return 0;
}
