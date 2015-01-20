#include "img_iter.h"


img_iter::img_iter(const Image& img, const int pc, const int vc, bool dummy)
: background(255, 255, 255), original(img), canvas(img.width(), img.height()),
  pm(pc, vc, img.width(), img.height()), maxAccuracy(getMaxAccuracy(img)),
  blockCountX(img.width() % blockSize == 0 ? img.width() / blockSize : img.width() / blockSize + 1),
  blockCountY(img.height() % blockSize == 0 ? img.height() / blockSize : img.height() / blockSize + 1) {
	(void)dummy;
	polygons.reserve(pc);
	blocks.reserve(blockCountX);
	for (int i = 0; i < blockCountX; ++i) {
		blocks.emplace_back();
		blocks[i].reserve(blockCountY);
		for (int j = 0; j < blockCountY; ++j)
			blocks[i].emplace_back();
	}
}


img_iter::img_iter(const Image& img, const int pc, const int vc)
: img_iter(img, pc, vc, true) {
	for (int i = 0; i < pc; ++i) {
		polygons.emplace_back(pm);
		polygons.back().setIndex(i);
	}
	init();
}


img_iter::img_iter(const Image& img, const DNA& d)
: img_iter(img, d.polyCount, d.vertCount, true) {
	int i = 0;
	for (auto it = d.data.begin(); it != d.data.end(); ++it, ++i) {
		polygons.emplace_back(pm, *it);
		polygons.back().setIndex(i);
	}
	init();
}


void img_iter::init() {
	// draw polygons on canvas
	canvas.clear(background);
	for (auto it = polygons.cbegin(); it != polygons.cend(); ++it) {
		canvas.setColor((*it).getColor());
		canvas.setAlpha((*it).getAlpha());
		canvas.fill((*it).getPolygon());
	}
	// copy canvas to best
	best = canvas.getImage();
	// set block accuracy
	for (int i = 0; i < blockCountX; ++i) {
		for (int j = 0; j < blockCountY; ++j) {
			blocks[i][j].acc = blockAccuracy(i, j);
		}
	}
	fit = getFitness();
	// set block polygon order
	BlockGroup bg;
	for (auto it = polygons.begin(); it != polygons.end(); ++it) {
		const auto index = (*it).getIndex();
		intersectIndex((*it).getBounds(), bg);
		for (int i = bg.iLo; i <= bg.iHi; ++i) {
			for (int j = bg.jLo; j <= bg.jHi; ++j)
				blocks[i][j].polygons.emplace(index);
		}
	}
	assert(validBlocks());

	start = std::chrono::high_resolution_clock::now();
}


// assumes all vertices are >= 0
void img_iter::iterate() {
	++iter;
	BlockGroup bg;
	std::set<Index2D> changes;	// changed blocks from this iteration
	IterPoly& ip = polygons[pm.randPolyIndex()];
	Rectangle bounds1{ip.getBounds()};
	intersectIndex(bounds1, bg);
	addBlockSet(changes, bg);
	ip.mutate();
	const bool sizeMutation = sizeChange(ip.lastMutation());
	Rectangle bounds2{ip.getBounds()};
	if (sizeMutation) {
		updateBlockPolygon(bg, ip, false);	// remove poly from original blocks
		intersectIndex(bounds2, bg);
		updateBlockPolygon(bg, ip, true);	// add poly to new blocks

		addBlockSet(changes, bg);
	}
	
	// draw and recalc fitness of changed blocks
	float old_acc_sum = 0;
	float new_acc_sum = 0;
	std::vector<float> new_acc;
	new_acc.reserve(changes.size());
	for (auto it = changes.cbegin(); it != changes.cend(); ++it) {
		drawBlock((*it).first, (*it).second);
		old_acc_sum += blocks[(*it).first][(*it).second].acc;
		new_acc.push_back(blockAccuracy((*it).first, (*it).second));
		new_acc_sum += new_acc.back();
	}

	if (new_acc_sum > old_acc_sum) {	// improvement
		++imp;
		// set new fitness of changed blocks and copy blocks to best
		int i = 0;
		for (auto it = changes.cbegin(); it != changes.cend(); ++it, ++i) {
			blocks[(*it).first][(*it).second].acc = new_acc[i];
			copyBlock(best, canvas, *it);
		}
		fit = getFitness();		
	}
	else {
		if (sizeMutation) {
			intersectIndex(bounds2, bg);
			updateBlockPolygon(bg, ip, false);	// remove poly from new blocks
			intersectIndex(bounds1, bg);
			updateBlockPolygon(bg, ip, true);	// add poly to original blocks
		}
		ip.undo();
	}

	assert(validBlocks());
}


// iterate until improvement has been made
void img_iter::improve() {
	const auto improvements = imp;
	while (imp == improvements)
		iterate();
}


int img_iter::iterations() const {
	return iter;
}


int img_iter::improvements() const {
	return imp;
}


float img_iter::fitness() const {
	return fit;
}


// time (seconds) since constructor called
int img_iter::runtime() const {
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now()-start).count();
}


// copy of best image
Image img_iter::getImage() const {
	return best;
}


// reference to improving image (for Viewer)
const Image& img_iter::bestImage() const {
	return best;
}


DNA img_iter::getDNA() const {
	DNA d{polygons.size(), polygons.front().getPolygon().size()};
	for (auto it = polygons.cbegin(); it != polygons.cend(); ++it)
		d.add((*it).getPolygon(), (*it).getColor(), (*it).getAlpha());
	return d;
}


void img_iter::drawBlock(const int i, const int j) {
	Rectangle mask;
	mask.x0 = i * blockSize;
	mask.y0 = j * blockSize;
	mask.x1 = std::min(mask.x0 + blockSize - 1, original.width() - 1);
	mask.y1 = std::min(mask.y0 + blockSize - 1, original.height() - 1);
	// reset block
	canvas.setColor(background);
	canvas.setAlpha(1.0);
	canvas.fill(mask);
	// draw block
	const auto& block = blocks[i][j];
	for (auto it = block.polygons.cbegin(); it != block.polygons.cend(); ++it) {
		const auto& ip = polygons[*it];
		canvas.setColor(ip.getColor());
		canvas.setAlpha(ip.getAlpha());
		canvas.fill(ip.getPolygon(), mask);
	}
}


float img_iter::getMaxAccuracy(const Image& img) {
	return img.width() * img.height();
}


// returns [0, 1], 1 being equal
float img_iter::getAccuracy(const Color& c1, const Color& c2) {
	return 1.0f - static_cast<float>(getDiff(c1.R, c2.R) + getDiff(c1.G, c2.G) + getDiff(c1.B, c1.B)) / (255 * 3);
}


int img_iter::getDiff(const Color::ColorChannel a, const Color::ColorChannel b) {
	return std::abs(static_cast<int>(a) - static_cast<int>(b));
}


float img_iter::getFitness() const {
	float accuracy = 0;
	for (int i = 0; i < blockCountX; ++i) {
		for (int j = 0; j < blockCountY; ++j)
			accuracy += blocks[i][j].acc;
	}
	return accuracy / maxAccuracy;
}


float img_iter::blockAccuracy(const int i, const int j) const {
	float accuracy = 0;
	const int xLim = std::min((i + 1) * blockSize, original.width());
	const int yLim = std::min((j + 1) * blockSize, original.height());
	for (int x = i * blockSize; x < xLim; ++x) {
		for (int y = j * blockSize; y < yLim; ++y)
			accuracy += getAccuracy(original.get(x, y), canvas.getPoint(x, y));
	}
	return accuracy;
}


bool img_iter::sizeChange(const Mutation m) {
	switch (m) {
	case Mutation::X:
	case Mutation::Y:
		return true;
	default:
		return false;
	}
}


void img_iter::intersectIndex(const Rectangle& r, BlockGroup& b) const {
	b.iLo = r.x0 / blockSize;
	b.iHi = r.x1 / blockSize;
	b.jLo = r.y0 / blockSize;
	b.jHi = r.y1 / blockSize;
	assert(b.iLo >= 0);
	assert(b.iHi >= 0);
	assert(b.jLo >= 0);
	assert(b.jHi >= 0);
	assert(b.iLo <= b.iHi);
	assert(b.jLo <= b.jHi);
	assert(b.iHi < blockCountX);
	assert(b.jHi < blockCountY);
}


// update which blocks reference this polygon
void img_iter::updateBlockPolygon(const BlockGroup& bg, const IterPoly& ip, const bool add) {
	const auto index = ip.getIndex();
	if (add) {
		for (int i = bg.iLo; i <= bg.iHi; ++i) {
			for (int j = bg.jLo; j <= bg.jHi; ++j)
				blocks[i][j].polygons.emplace(index);
		}
	}
	else {
		for (int i = bg.iLo; i <= bg.iHi; ++i) {
			for (int j = bg.jLo; j <= bg.jHi; ++j)
				blocks[i][j].polygons.erase(index);
		}
	}
}


void img_iter::addBlockSet(std::set<Index2D>& s, const BlockGroup& bg) {
	for (int i = bg.iLo; i <= bg.iHi; ++i) {
		for (int j = bg.jLo; j <= bg.jHi; ++j)
			s.emplace(i, j);
	}
}


// copy block from canvas to best
void img_iter::copyBlock(Image& img, const Canvas& can, const Index2D& index) {
	const int xLim = std::min((index.first + 1) * blockSize, original.width());
	const int yLim = std::min((index.second + 1) * blockSize, original.height());
	for (int x = index.first * blockSize; x < xLim; ++x) {
		for (int y = index.second * blockSize; y < yLim; ++y)
			img.set(x, y, can.getPoint(x, y));
	}
}


bool img_iter::validBlocks(void) const {
	BlockGroup bg;
	for (auto it = polygons.cbegin(); it != polygons.cend(); ++it) {
		const auto index = (*it).getIndex();
		intersectIndex((*it).getBounds(), bg);
		for (int i = bg.iLo; i <= bg.iHi; ++i) {
			for (int j = bg.jLo; j <= bg.jHi; ++j) {
				const auto& polySet = blocks[i][j].polygons;
				if (polySet.count(index) == 0)
					return false;
			}
		}
	}
	return true;
}
