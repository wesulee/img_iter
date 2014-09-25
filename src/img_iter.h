#pragma once

#include "canvas.h"
#include "dna.h"
#include "poly_mutator.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <set>
#include <string>
#include <vector>


class img_iter {
	struct BlockGroup {
		int iLo, iHi, jLo, jHi;	// indices (INCLUSIVE)
	};
	struct Block {
		float acc = 0;
		std::set<int> polygons;
	};
	typedef std::pair<int, int> Index2D;
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
	const Image& bestImage(void) const;
	DNA getDNA(void) const;
private:
	img_iter(const Image&, const int, const int, bool);
	void init();
	void drawPolygons(void);
	void drawBlock(const int, const int);
	static float getMaxAccuracy(const Image&);
	static float getAccuracy(const Color&, const Color&);
	static int getDiff(const Color::ColorChannel, const Color::ColorChannel);
	float getFitness(void) const;
	float blockAccuracy(const int, const int) const;
	static bool sizeChange(const Mutation);
	void intersectIndex(const Rectangle&, BlockGroup&) const;
	void updateBlockPolygon(const BlockGroup&, const IterPoly&, const bool);
	static void addBlockSet(std::set<Index2D>&, const BlockGroup&);
	void copyBlock(Image&, const Canvas&, const Index2D&);
	bool validBlocks(void) const;

	static constexpr int blockSize = 50;	// px
	static constexpr float maxBlockAccuracy = blockSize * blockSize;
	const Color background;
	const Image original;
	Image best;
	Canvas canvas;
	poly_mutator pm;
	const float maxAccuracy;
	std::vector<IterPoly> polygons;
	std::vector<std::vector<Block>> blocks;
	const int blockCountX;
	const int blockCountY;
	unsigned int iter = 0;
	unsigned int imp = 0;
	float fit = 0;
	std::chrono::high_resolution_clock::time_point start;
};
