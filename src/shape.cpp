#include "shape.h"


Point::Point(const Point& p) : x(p.x), y(p.y) {
}


Point::Point(const int x, const int y) : x(x), y(y) {
}


Rectangle::Rectangle(const Point& p) : x0(p.x), y0(p.y), x1(p.x), y1(p.y) {
}


Rectangle::Rectangle(const Rectangle& r) : x0(r.x0), y0(r.y0), x1(r.x1), y1(r.y1) {
}


Rectangle Rectangle::getBounds() const {
	return *this;
}


bool Rectangle::operator==(const Rectangle& r) const {
	return (x0 == r.x0 && y0 == r.y0 && x1 == r.x1 && y1 == r.y1);
}


bool Rectangle::contains(const Point& p) const {
	return (
		(p.x >= x0)
		&& (p.x <= x1)
		&& (p.y >= y0)
		&& (p.y <= y1)
	);
}


int Rectangle::area() const {
	return ((std::abs(x1 - x0) + 1) * (std::abs(y1 - y0) + 1));
}


std::ostream& operator<<(std::ostream& os, const Point& p) {
	os << '(' << p.x << ", " << p.y << ')';
	return os;
}


std::ostream& operator<<(std::ostream& os, const Rectangle& r) {
	os << "x0: " << r.x0 << " y0: " << r.y0 << " x1: " << r.x1 << " y1: " << r.y1;
	return os;
}


void ShapeHelper::updateRectBoundsX(Rectangle& rect, const int x) {
	if (x < rect.x0)
		rect.x0 = x;
	else if (x > rect.x1)
		rect.x1 = x;
}


void ShapeHelper::updateRectBoundsY(Rectangle& rect, const int y) {
	if (y < rect.y0)
		rect.y0 = y;
	else if (y > rect.y1)
		rect.y1 = y;
}


Rectangle ShapeHelper::joinRectangles(const Rectangle& rect1, const Rectangle& rect2) {
	Rectangle ret{rect1};
	updateRectBoundsX(ret, rect2.x0);
	updateRectBoundsY(ret, rect2.y0);
	updateRectBoundsX(ret, rect2.x1);
	updateRectBoundsY(ret, rect2.y1);
	return ret;
}


bool ShapeHelper::intersects(const Rectangle& rect1, const Rectangle& rect2) {
	return (
		(rect1.x0 < rect2.x1)
		&& (rect1.x1 > rect2.x0)
		&& (rect1.y0 < rect2.y1)
		&& (rect1.y1 > rect2.y0)
	);
}


bool ShapeHelper::validRect(const Rectangle& r) {
	return (
		(r.x0 >= 0)
		&& (r.y0 >= 0)
		&& (r.x1 >= 0)
		&& (r.y1 >= 0)
		&& (r.x1 >= r.x0)
		&& (r.y1 >= r.y0)
	);
}
