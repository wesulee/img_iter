#include "polygon.h"


Polygon::Polygon(const Container& c) : v(c) {
	if (!c.empty()) {
		auto it = c.cbegin();
		bounds = Rectangle(*it);
		assert(bounds.contains(*it));
		for (++it; it != c.cend(); ++it) {
			ShapeHelper::updateRectBoundsX(bounds, (*it).x);
			ShapeHelper::updateRectBoundsY(bounds, (*it).y);
			assert(bounds.contains(*it));
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


Point Polygon::get(const std::size_t i) const {
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
	assert(bounds.contains(p));
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
	assert(bounds.contains(Point{x, y}));
}


void Polygon::setX(const std::size_t i, const int x) {
	useCache = false;
	v[i].x = x;
	bounds = newBounds();
	assert(bounds.contains(v[i]));
}


void Polygon::setY(const std::size_t i, const int y) {
	useCache = false;
	v[i].y = y;
	bounds = newBounds();
	assert(bounds.contains(v[i]));
}


Rectangle Polygon::getBounds() const {
	return bounds;
}


const Polygon::Container& Polygon::vertices() const {
	return v;
}


// https://www.cs.rit.edu/~icss571/filling/index.html
// note: fillLines returned are always in ascending order
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
		Point prevPoint{*it};
		Point currPoint;
		++it;
		std::size_t checked = 0;
		while (checked < v.size()) {
			++checked;
			currPoint = *it;
			if (currPoint.y > scanMax)
				scanMax = currPoint.y;

			// add to globalTable only if slope != 0
			if ((prevPoint.y - currPoint.y) != 0) {
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
			}
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
	bool state;
	float dummy;	// for modf
	for (int scan = globalTable.front().yMin; scan <= scanMax; ++scan) {
		tmpLine.y = scan;
		tmpLine.xList.clear();
		state = false;
		// move edges from global to active
		while (!globalTable.empty() && (globalTable.front().yMin == scan)) {
			active.push_back(globalTable.front());
			globalTable.pop_front();
		}

		for (auto it = active.begin(); it != active.end(); ++it, state = !state) {
			if (state) {
				frac = std::abs(std::modf((*it).x, &dummy));
				if (frac >= 0.5)
					tmpLine.xList.push_back(static_cast<int>((*it).x) + 1);
				else
					tmpLine.xList.push_back(static_cast<int>((*it).x));
			}
			else {
				frac = std::abs(std::modf((*it).x, &dummy));
				if (frac < 0.5)
					tmpLine.xList.push_back(static_cast<int>((*it).x));
				else
					tmpLine.xList.push_back(static_cast<int>((*it).x) + 1);
			}

			if ((*it).yMax == scan + 1)	// remove current edge from active if at edge limit
				it = --active.erase(it);
			else	// update x
				(*it).x += (*it).mInv;
		}
		std::sort(tmpLine.xList.begin(), tmpLine.xList.end());
		fillLines.push_back(tmpLine);
	}

	#ifndef NDEBUG
	assert(!fillCache.empty());
	assert(bounds.y0 == fillCache.front().y);
	assert(bounds.y1 == fillCache.back().y);
	assert(fillCache.back().y - fillCache.front().y >= 0);
	assert(static_cast<std::size_t>(fillCache.back().y - fillCache.front().y + 1) == fillCache.size()) ;
	// check y is sorted
	auto it = fillCache.cbegin();
	auto prevY = (*it).y;
	for (++it; it != fillCache.cend(); ++it) {
		assert(prevY <= (*it).y);
		assert((*it).y - prevY == 1);
		prevY = (*it).y;
		assert((*it).xList.size() % 2 == 0);
	}
	#endif

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


Rectangle Polygon::newBounds() const {
	assert(!v.empty());
	auto it = v.cbegin();
	Rectangle r{*it};
	++it;
	while (it != v.cend()) {
		ShapeHelper::updateRectBoundsX(r, (*it).x);
		ShapeHelper::updateRectBoundsY(r, (*it).y);
		++it;
	}
	return r;
}


std::ostream& operator<<(std::ostream& os, const Polygon& p) {
	const auto& vert = p.vertices();
	auto it = vert.cbegin();
	if (it != vert.cend()) {
		os << *it;
		++it;
		while (it != vert.cend()) {
			os << ", " << *it;
			++it;
		}
	}
	return os;
}
