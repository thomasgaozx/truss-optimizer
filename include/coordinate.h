#ifndef COORDINATE_H_
#define COORDINATE_H_

struct Coordinate {
    Coordinate() : x{}, y{} {}
    Coordinate(double _x, double _y) : x{ _x }, y{ _y } {}
    Coordinate(const Coordinate & other) : x{ other.x }, y{ other.y } {}
    Coordinate(Coordinate && other) : x(std::move(other.x)), y{ std::move(other.y) } {}
    
    Coordinate& operator=(const Coordinate & other) {
        x = other.x;
        y = other.y;
        return *this;
    }

    Coordinate& operator=(Coordinate && other) {
        x = std::move(other.x);
        y = std::move(other.y);
        return *this;
    }

    double x;
    double y;
};

#endif
