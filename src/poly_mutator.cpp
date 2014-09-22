#include "poly_mutator.h"


std::uniform_int_distribution<int> poly_mutator::distMut = std::uniform_int_distribution<int>(0, poly_mutator::MutationCount - 1);
std::uniform_int_distribution<uint_fast32_t> poly_mutator::distAlpha = std::uniform_int_distribution<uint_fast32_t>(0);
std::uniform_int_distribution<Color::ColorChannel> poly_mutator::distCol = std::uniform_int_distribution<Color::ColorChannel>{Color::minColorChannel, Color::maxColorChannel};


poly_mutator::poly_mutator(const int pc, const int vc, const int w, const int h)
: polyCount(pc), vertCount(vc), width(w), height(h) {
	std::random_device rd;
	re = std::default_random_engine(rd());
	distNorm = std::normal_distribution<float>{0.0, 0.4};
	distUni = std::uniform_real_distribution<float>{0.0, 1.0};
	distVertX = std::uniform_int_distribution<int>(0, w - 1);
	distVertY = std::uniform_int_distribution<int>(0, h - 1);
	distPolyIndex = std::uniform_int_distribution<std::size_t>(0, pc - 1);
	distVertIndex = std::uniform_int_distribution<std::size_t>(0, vc - 1);
}


/*
Generate a random simple polygon inside a rectangle
vert is number of vertices (>= 3)
width, height are dimensions of rectangle
spacing (0 - 1 inclusive) determines random spacing along circle
sharpness (0 - 1 inclusive) determines how spiky polygon will be
*/
Polygon poly_mutator::randSimplePoly(const float spacing, const float sharpness) {
	// get center of poly close to rectangle center with padding from edge
	const int cenX = static_cast<int>((width / 2) + (randNorm() * width / 3));
	const int cenY = static_cast<int>((height / 2) + (randNorm() * height / 3));
	// polygon vertices
	Polygon::Container v;
	v.reserve(vertCount);
	Point p;
	// begin generating vertices
	const float radInc = 2 * PI / vertCount;
	float rad = randUni() * 2 * PI;
	for (int vert = 0; vert < vertCount; ++vert, rad += radInc) {
		float radRand = rad + radInc * randNorm() / 2.1 * spacing;
		std::pair<float, float> intPoint = getPos(cenX, cenY, radRand, width, height);
		// vector from center to intersection
		float vx = intPoint.first - static_cast<float>(cenX);
		float vy = intPoint.second - static_cast<float>(cenY);
		// delta from center
		float vMult = randNorm();
		float dx = (vx / 2) * (1.0 + vMult * sharpness);
		float dy = (vy / 2) * (1.0 + vMult * sharpness);
		p.x = static_cast<int>(cenX + dx);
		p.y = static_cast<int>(cenY + dy);
		v.push_back(p);
	}
	return Polygon(v);
}


// actually random...
Polygon poly_mutator::randPoly() {
	Polygon::Container v;
	v.reserve(vertCount);
	Point p;
	for (int i = 0; i < vertCount; ++i) {
		p.x = distVertX(re);
		p.y = distVertY(re);
		v.push_back(p);
	}
	return Polygon{v};
}


Color poly_mutator::randColor() {
	return Color{randColChannel(), randColChannel(), randColChannel()};
}


Color::ColorChannel poly_mutator::randColChannel() {
	return distCol(re);
}


float poly_mutator::randAlpha() {
	return static_cast<float>(distAlpha(re)) / alphaDiv;
}


Mutation poly_mutator::randMutation() {
	switch(distMut(re)) {
	case 0:
		return Mutation::X;
	case 1:
		return Mutation::Y;
	case 2:
		return Mutation::R;
	case 3:
		return Mutation::G;
	case 4:
		return Mutation::B;
	case 5:
		return Mutation::A;
	default:
		return Mutation::X;	// shouldn't happen
	}
}


std::size_t poly_mutator::randPolyIndex() {
	return distPolyIndex(re);
}


std::size_t poly_mutator::randVertIndex() {
	return distVertIndex(re);
}


int poly_mutator::randVertX() {
	return distVertX(re);
}


int poly_mutator::randVertY() {
	return distVertY(re);
}


// random float (-1, 1)
float poly_mutator::randNorm() {
	float ret;
	do {
		ret = distNorm(re);
	} while (ret >= 1.0 || ret <= -1.0);
	return ret;
}


float poly_mutator::randUni() {
	return distUni(re);
}


std::pair<float, float> poly_mutator::getPos(const int cx, const int cy, const float rad, const int w, const int h) {
	float ix;	// intersection
	float iy;
	float fcx = static_cast<float>(cx);
	float fcy = static_cast<float>(cy);
	// end points of a line from center that extends beyond rect
	float ex = std::cos(rad) * (w + h);
	float ey = std::sin(rad) * (w + h);
	// intersect top
	if (lineIntersect(fcx, fcy, ex, ey, -1, 0, w, 0, ix, iy))
		return std::pair<float, float>(ix, iy);
	// intersect right
	if (lineIntersect(fcx, fcy, ex, ey, w - 1, -1, w - 1, h, ix, iy))
		return std::pair<float, float>(ix, iy);
	// intersect bottom
	if (lineIntersect(fcx, fcy, ex, ey, -1, h - 1, w, h - 1, ix, iy))
		return std::pair<float, float>(ix, iy);
	// intersect left
	if (lineIntersect(fcx, fcy, ex, ey, 0, -1, 0, h, ix, iy))
		return std::pair<float, float>(ix, iy);

	// something went wrong...
	return std::pair<float, float>(ix, iy);
}


// http://stackoverflow.com/a/1968345
bool poly_mutator::lineIntersect(float a0x, float a0y, float a1x, float a1y, float b0x, float b0y, float b1x, float b1y, float& ix, float& iy) {
	float s1_x = a1x - a0x;
	float s1_y = a1y - a0y;
	float s2_x = b1x - b0x;
	float s2_y = b1y - b0y;
	float s = (-s1_y * (a0x - b0x) + s1_x * (a0y - b0y)) / (-s2_x * s1_y + s1_x * s2_y);
	float t = ( s2_x * (a0y - b0y) - s2_y * (a0x - b0x)) / (-s2_x * s1_y + s1_x * s2_y);

	if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
		// Collision detected
		ix = a0x + (t * s1_x);
		iy = a0y + (t * s1_y);
		return true;
	}

	return false; // No collision
}


IterPoly::IterPoly(poly_mutator& pm)
: mutator(pm), p(pm.randPoly()), c(pm.randColor()), a(pm.randAlpha()) {
}


IterPoly::IterPoly(poly_mutator& pm, const PolyDNA& d)
: mutator(pm), p(), c(d.color), a(d.alpha) {
	for (auto it = d.v.cbegin(); it != d.v.cend(); ++it)
		p.add(*it);
}


void IterPoly::mutate() {
	m = mutator.randMutation();
	switch (m) {
	case Mutation::X:
		index = mutator.randVertIndex();
		pp = p.get(index).x;
		p.setX(index, mutator.randVertX());
		break;
	case Mutation::Y:
		index = mutator.randVertIndex();
		pp = p.get(index).y;
		p.setY(index, mutator.randVertY());
		break;
	case Mutation::R:
		cc = c.R;
		c.R = mutator.randColChannel();
		break;
	case Mutation::G:
		cc = c.G;
		c.G = mutator.randColChannel();
		break;
	case Mutation::B:
		cc = c.B;
		c.B = mutator.randColChannel();
		break;
	case Mutation::A:
		a2 = a;
		a = mutator.randAlpha();
		break;
	default:
		break;
	}
}


// undo the most recent change
// should be called only between mutate()
void IterPoly::undo() {
	switch (m) {
	case Mutation::X:
		swapVertX();
		break;
	case Mutation::Y:
		swapVertY();
		break;
	case Mutation::R:
		swap(c.R, cc);
		break;
	case Mutation::G:
		swap(c.G, cc);
		break;
	case Mutation::B:
		swap(c.B, cc);
		break;
	case Mutation::A:
		swap(a, a2);
		break;
	default:
		break;
	}
}


const Polygon& IterPoly::getPolygon() const {
	return p;
}


const Color& IterPoly::getColor() const {
	return c;
}


float IterPoly::getAlpha() const {
	return a;
}


Mutation IterPoly::lastMutation() const {
	return m;
}


Rectangle IterPoly::getBounds() const {
	return p.getBounds();
}


template <class T>
void IterPoly::swap(T& a, T& b) {
	T tmp = a;
	a = b;
	b = tmp;
}


void IterPoly::swapVertX() {
	const int tmp = pp;
	pp = p.get(index).x;
	p.setX(index, tmp);
}


void IterPoly::swapVertY() {
	const int tmp = pp;
	pp = p.get(index).y;
	p.setY(index, tmp);
}
