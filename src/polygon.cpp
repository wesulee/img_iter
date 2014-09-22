#include "polygon.h"


Polygon::Polygon(const Container& c) : v(c) {
	if (!c.empty()) {
		auto it = c.cbegin();
		bounds = Rectangle(*it);
		for (++it; it != c.cend(); ++it) {
			ShapeHelper::updateRectBoundsX(bounds, (*it).x);
			ShapeHelper::updateRectBoundsY(bounds, (*it).y);
		}
	}
}


std::size_t Polygon::size() const {
	return v.size();
}


void Polygon::clear() {
	useCache = false;
	v.clear();
}


bool Polygon::empty() const {
	return v.empty();
}


const Point& Polygon::get(const std::size_t i) const {
	return v[i];
}


void Polygon::add(const Point& p) {
	useCache = false;
	if (v.empty()) {
		bounds = Rectangle(p);
	}
	else {
		ShapeHelper::updateRectBoundsX(bounds, p.x);
		ShapeHelper::updateRectBoundsY(bounds, p.y);
	}
	v.push_back(p);
}


void Polygon::add(const int x, const int y) {
	useCache = false;
	if (v.empty()) {
		bounds = Rectangle(Point{x, y});
	}
	else {
		ShapeHelper::updateRectBoundsX(bounds, x);
		ShapeHelper::updateRectBoundsY(bounds, y);
	}
	v.push_back(Point{x, y});
}


void Polygon::setX(const std::size_t i, const int x) {
	useCache = false;
	v[i].x = x;
	ShapeHelper::updateRectBoundsX(bounds, x);
}


void Polygon::setY(const std::size_t i, const int y) {
	useCache = false;
	v[i].y = y;
	ShapeHelper::updateRectBoundsX(bounds, y);
}


Rectangle Polygon::getBounds() const {
	return bounds;
}


const Polygon::Container& Polygon::vertices() const {
	return v;
}


// https://www.cs.rit.edu/~icss571/filling/index.html
const std::vector<PolyHelper::FillLine>& Polygon::fillDetails() const {
	using namespace PolyHelper;
	if (useCache)
		return fillCache;

	fillCache.clear();
	auto& fillLines = fillCache;
	std::list<FillEdge> globalTable;
	int scanMax;
	FillEdge tmp;
	{
		int xMin;
		int xMax;
		auto it = v.cbegin();
		scanMax = (*it).y;	// default value
		Point prevPoint;
		Point currPoint;
		prevPoint = *it;
		++it;
		std::size_t checked = 0;
		while (checked < v.size()) {
			++checked;
			currPoint = *it;
			if (currPoint.y > scanMax)
				scanMax = currPoint.y;

			if ((prevPoint.y - currPoint.y) == 0)	// skip slope 0
				continue;

			if (prevPoint.y < currPoint.y) {
				xMin = prevPoint.x;
				xMax = currPoint.x;
				tmp.yMin = prevPoint.y;
				tmp.yMax = currPoint.y;
				tmp.x = static_cast<float>(prevPoint.x);
			}
			else {
				xMin = currPoint.x;
				xMax = prevPoint.x;
				tmp.yMin = currPoint.y;
				tmp.yMax = prevPoint.y;
				tmp.x = static_cast<float>(currPoint.x);
			}

			tmp.mInv = static_cast<float>(xMax - xMin) / (tmp.yMax - tmp.yMin);
			globalTable.push_back(tmp);
			prevPoint = currPoint;
			++it;
			if (it == v.cend())
				it = v.cbegin();
		}
	}

	globalTable.sort();

	std::list<FillEdge> active;
	FillLine tmpLine;
	float frac;
	int state;
	float dummy;	// for modf
	for (int scan = globalTable.front().yMin; scan < scanMax; ++scan) {
		tmpLine.y = scan;
		tmpLine.xList.clear();
		state = 0;
		// move edges from global to active
		while (!globalTable.empty() && (globalTable.front().yMin == scan)) {
			active.push_back(globalTable.front());
			globalTable.pop_front();
		}

		for (auto it = active.begin(); it != active.end(); ++it) {
			switch (state) {
			case 0:	// beginning of draw
				frac = std::abs(std::modf((*it).x, &dummy));
				if (frac < 0.5)
					tmpLine.xList.push_back(static_cast<int>((*it).x));
				else
					tmpLine.xList.push_back(static_cast<int>((*it).x) + 1);
				break;
			case 1:	// end of draw
				frac = std::abs(std::modf((*it).x, &dummy));
				if (frac >= 0.5)
					tmpLine.xList.push_back(static_cast<int>((*it).x) + 1);
				else
					tmpLine.xList.push_back(static_cast<int>((*it).x));
				break;
			case 2:	// not drawing
				break;
			default:
				break;	// shouldn't happen
			}
			state = (state + 1) % 2;
			if ((*it).yMax == scan + 1)	// remove current edge from active if at edge limit
				it = --active.erase(it);
			else	// update x
				(*it).x += (*it).mInv;
		}
		tmpLine.xList.sort();
		fillLines.push_back(tmpLine);
	}

	useCache = true;
	return fillCache;
}


bool PolyHelper::FillEdge::operator<(const PolyHelper::FillEdge& that) const {
	if (yMin < that.yMin)
		return true;
	else if (yMin == that.yMin)
		return (x < that.x);
	else
		return false;
}
