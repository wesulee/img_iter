#pragma once


class Rectangle;


struct Point {
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
	Rectangle(const Point& p) 
	: x0(p.x), y0(p.y), x1(p.x), y1(p.y) {}
	Rectangle(const Rectangle& r)
	: x0(r.x0), y0(r.y0), x1(r.x1), y1(r.y1) {}
	~Rectangle() = default;
	Rectangle getBounds() const override {
		return *this;
	}
	Rectangle& operator=(const Rectangle&) = default;
	bool operator==(const Rectangle& r) const {
		return (x0 == r.x0 && y0 == r.y0 && x1 == r.x1 && y1 == r.y1);
	}

	int x0;
	int y0;
	int x1;
	int y1;
};


namespace ShapeHelper {
	void updateRectBoundsX(Rectangle&, const int);
	void updateRectBoundsY(Rectangle&, const int);
	Rectangle joinRectangles(const Rectangle&, const Rectangle&);
}
