#include <cmath>
#include <string>
#include <vector>

class Point
{
public:
    Point() {}
    Point(double x, double y) : x(x), y(y) {}

    ~Point() {}

    Point operator-(const Point& point) const
    {
        return Point{this->x - point.x, this->y - point.y};
    }

    double getNolm() const
    {
        return std::sqrt(this->x * this->x + this->y * this->y);
    }

    double x;
    double y;
};
