#include <iostream>
#include "Point.h"
#include "CRect.h"
#include "Transform.h"

using std::cout;
using std::endl;

int main() {
    const double PI = 3.14159265358979;
    Rect r(Point(1,1), Point(5,8));
    cout << r << endl
         << r.width() << endl
         << r.height() << endl
         << r.area() << endl;

    cout << Transform() << endl;
    cout << Transform::Translate(4, 5) << endl;
    cout << Transform::Scale(2.0, 2.0) << endl;
    cout << Transform::Rotate( PI) << endl;
    cout << Transform() * Transform::Translate(3, 2) << endl;

    Point p(2, 3);
    cout << p << endl;
    cout << Transform::Translate(2, 3) * p << endl;
    cout << Transform::Scale(4.0, 2.0) * p << endl;
    cout << Transform::Rotate(PI) * Point(3, 1) << endl;

    cout << Transform::Translate(2, 3) * Transform::Scale(2.0, 3.0) << endl;
    cout << Transform::Rotate(PI) * Transform::Translate(2, 3) * Transform::Scale(2.0, 3.0) * Point(3, 2) << endl;

    cout << r << endl;
    cout << Transform::Scale(0.5, 0.5) * r << endl;
    cout << Transform::Translate(-r.p1_) * r << endl;
    cout << Transform::Translate(-r.p1_) * Transform::Scale(0.5, 0.5) * r << endl;
    cout << Transform::Scale(0.5, 0.5) * Transform::Translate(-r.p1_) * r << endl;

    return 0;
}