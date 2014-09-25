#pragma once

#include <cmath>
#include <iostream>


class Rectangle;


class Point {
public:
	Point() = default;
	Point(const Point&);
	Point(const int, const int);
	~Point() = default;

	int x;
	int y;
};


class Shape {
public:
	Shape() = default;
	virtual ~Shape() {}
	virtual Rectangle getBounds(void) const = 0;
};


class Rectangle : public Shape {
public:
	Rectangle() = default;
	Rectangle(const Point&);
	Rectangle(const Rectangle&);
	~Rectangle() = default;
	Rectangle getBounds(void) const override;
	Rectangle& operator=(const Rectangle&) = default;
	bool operator==(const Rectangle&) const;
	bool contains(const Point&) const;
	int area(void) const;

	int x0;
	int y0;
	int x1;
	int y1;
};


std::ostream& operator<<(std::ostream&, const Point&);
std::ostream& operator<<(std::ostream&, const Rectangle&);


namespace ShapeHelper {
	void updateRectBoundsX(Rectangle&, const int);
	void updateRectBoundsY(Rectangle&, const int);
	Rectangle joinRectangles(const Rectangle&, const Rectangle&);
	bool intersects(const Rectangle&, const Rectangle&);
	bool validRect(const Rectangle&);
}
