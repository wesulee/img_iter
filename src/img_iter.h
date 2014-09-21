#pragma once

#include "canvas.h"
#include "dna.h"
#include "poly_mutator.h"
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
	void drawPolygons(void);
	static float getMaxAccuracy(const Image&);
	static float getAccuracy(const Color&, const Color&);
	static int getDiff(const Color::ColorChannel, const Color::ColorChannel);
	float getFitness(void) const;

	const Image original;
	Canvas canvas;
	poly_mutator pm;
	const float maxAccuracy;
	std::vector<IterPoly> polygons;
	unsigned int iter = 0;
	unsigned int imp = 0;
	float fit = 0;
	std::chrono::high_resolution_clock::time_point start;
};
