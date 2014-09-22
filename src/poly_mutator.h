#pragma once

#include "color.h"
#include "dna.h"
#include "polygon.h"
#include <cmath>
#include <limits>
#include <random>
#include <utility>


enum class Mutation {X, Y, R, G, B, A};


// helper class to implement IterPoly
class poly_mutator {
public:
	static constexpr int MutationCount = 6;
	poly_mutator(const int, const int, const int, const int);
	~poly_mutator() = default;
	Polygon randSimplePoly(const float, const float);
	Polygon randPoly(void);
	Color randColor(void);
	Color::ColorChannel randColChannel(void);
	float randAlpha(void);
	Mutation randMutation(void);
	std::size_t randPolyIndex(void);
	std::size_t randVertIndex(void);
	int randVertX(void);
	int randVertY(void);
private:
	float randNorm(void);
	float randUni(void);
	std::pair<float, float> getPos(const int, const int, const float, const int, const int);
	static bool lineIntersect(float, float, float, float, float, float, float, float, float&, float&);

	const int polyCount;
	const int vertCount;
	const int width;
	const int height;
	static constexpr float PI = std::atan(1.0) * 4;
	static constexpr uint_fast32_t alphaDiv = std::numeric_limits<uint_fast32_t>::max();
	static std::uniform_int_distribution<uint_fast32_t> distAlpha;
	static std::uniform_int_distribution<int> distMut;
	static std::uniform_int_distribution<Color::ColorChannel> distCol;
	std::default_random_engine re;
	std::normal_distribution<float> distNorm;
	std::uniform_real_distribution<float> distUni;
	std::uniform_int_distribution<int> distVertX;
	std::uniform_int_distribution<int> distVertY;
	std::uniform_int_distribution<std::size_t> distPolyIndex;
	std::uniform_int_distribution<std::size_t> distVertIndex;
};


// Note: all vertices must be >= 0
class IterPoly {
public:
	IterPoly(poly_mutator&);
	IterPoly(poly_mutator&, const PolyDNA&);
	~IterPoly() = default;
	void mutate(void);
	void undo(void);
	const Polygon& getPolygon(void) const;
	const Color& getColor(void) const;
	float getAlpha(void) const;
	Mutation lastMutation(void) const;
	Rectangle getBounds(void) const;
private:
	template <class T> static void swap(T&, T&);
	void swapVertX(void);
	void swapVertY(void);
	void checkMinMaxX(const int);
	void checkMinMaxY(const int);

	poly_mutator& mutator;
	Polygon p;
	Color c;
	float a;
	// members for implementing undo()
	std::size_t index;	// vertex index
	int pp;		// old x or y coordinate
	Color::ColorChannel cc;
	float a2;
	Mutation m;
};
