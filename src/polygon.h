#pragma once

#include "shape.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include <vector>


namespace PolyHelper {
	struct FillEdge {
		int yMin;
		int yMax;
		float x;
		float mInv;

		bool operator<(const FillEdge&) const;
	};
	struct FillLine {
		int y;
		std::vector<int> xList;
	};
}


class Polygon : public Shape {
public:
	typedef std::vector<Point> Container;
	Polygon() = default;
	Polygon(const Container&);
	Polygon(const Polygon&) = default;
	~Polygon() = default;
	std::size_t size(void) const;
	void clear(void);
	bool empty(void) const;
	Point get(const std::size_t) const;
	void add(const Point&);
	void add(const int, const int);
	void setX(const std::size_t, const int);
	void setY(const std::size_t, const int);
	Rectangle getBounds(void) const override;
	const Container& vertices(void) const;
	const std::vector<PolyHelper::FillLine>& fillDetails(void) const;
private:
	Rectangle newBounds(void) const;

	Container v;
	Rectangle bounds;
	mutable std::vector<PolyHelper::FillLine> fillCache;
	mutable bool useCache = false;
};


std::ostream& operator<<(std::ostream&, const Polygon&);
