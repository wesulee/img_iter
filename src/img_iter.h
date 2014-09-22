#pragma once

#include "canvas.h"
#include "dna.h"
#include "poly_mutator.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>


class img_iter {
public:
	img_iter(const Image&, const int, const int);
	img_iter(const Image&, const DNA&);
	~img_iter() = default;
	void iterate(void);
	void improve(void);
	int iterations(void) const;
	int improvements(void) const;
	float fitness(void) const;
	int runtime(void) const;
	Image getImage(void) const;
	DNA getDNA(void) const;
	const Canvas& getCanvas(void) const;
private:
	img_iter(const Image&, const int, const int, bool);
	void init();
	void drawPolygons(void);
	static float getMaxAccuracy(const Image&);
	static float getAccuracy(const Color&, const Color&);
	static int getDiff(const Color::ColorChannel, const Color::ColorChannel);
	float getFitness(void) const;
	float blockAccuracy(const int, const int) const;
	static int countBlocks(const Rectangle&);
	void recalcAccuracy(const Rectangle&);

	static constexpr int blockSize = 50;	// px
	static constexpr float maxBlockAccuracy = blockSize * blockSize;
	const Image original;
	Canvas canvas;
	poly_mutator pm;
	const float maxAccuracy;
	std::vector<IterPoly> polygons;
	std::vector<std::vector<float>> blockAcc;
	const int blockCountX;
	const int blockCountY;
	Rectangle changeRect;
	bool useChangeRect = false;
	unsigned int iter = 0;
	unsigned int imp = 0;
	float fit = 0;
	std::chrono::high_resolution_clock::time_point start;
};
