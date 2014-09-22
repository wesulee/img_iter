#include "img_iter.h"


img_iter::img_iter(const Image& img, const int pc, const int vc, bool dummy)
: original(img), canvas(img.width(), img.height()), pm(pc, vc, img.width(), img.height()),
  maxAccuracy(getMaxAccuracy(img)),
  blockCountX(img.width() % blockSize == 0 ? img.width() / blockSize : img.width() / blockSize + 1),
  blockCountY(img.height() % blockSize == 0 ? img.height() / blockSize : img.height() / blockSize + 1) {
	dummy = dummy;	// get rid of warning
	polygons.reserve(pc);
	blockAcc.reserve(blockCountX);
	for (int i = 0; i < blockCountX; ++i) {
		blockAcc.emplace_back();
		blockAcc[i].reserve(blockCountY);
	}
}


img_iter::img_iter(const Image& img, const int pc, const int vc)
: img_iter(img, pc, vc, true) {
	for (int i = 0; i < pc; ++i)
		polygons.emplace_back(pm);
	init();
}


img_iter::img_iter(const Image& img, const DNA& d)
: img_iter(img, d.polyCount, d.vertCount, true) {
	for (auto it = d.data.begin(); it != d.data.end(); ++it)
		polygons.emplace_back(pm, *it);
	init();
}


void img_iter::init() {
	drawPolygons();
	// set block accuracy
	for (int i = 0; i < blockCountX; ++i) {
		for (int j = 0; j < blockCountY; ++j) {
			blockAcc[i].push_back(blockAccuracy(i, j));
		}
	}
	fit = getFitness();
	start = std::chrono::high_resolution_clock::now();
}


// assumes all vertices are >= 0
void img_iter::iterate() {
	++iter;
	IterPoly& ip = polygons[pm.randPolyIndex()];
	Rectangle bounds1{ip.getBounds()};
	ip.mutate();
	drawPolygons();
	Rectangle bounds2{ShapeHelper::joinRectangles(bounds1, ip.getBounds())};
	if (useChangeRect) {	// recalc blocks from previous iteration
		Rectangle bounds4{ShapeHelper::joinRectangles(changeRect, bounds2)};
		// determine if faster to recalc two separate rectangles or a joined rectangle
		if (countBlocks(bounds2) + countBlocks(changeRect) < countBlocks(bounds4)) {
			recalcAccuracy(bounds2);
			recalcAccuracy(changeRect);
		}
		else {
			recalcAccuracy(bounds4);
		}
	}
	else {
		recalcAccuracy(bounds2);
	}

	const float mfit = getFitness();
	if (mfit > fit) {
		++imp;
		fit = mfit;
		useChangeRect = false;
	}
	else {
		ip.undo();
		changeRect = bounds2;
		useChangeRect = true;
	}
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


Image img_iter::getImage() const {
	return canvas.getImage();
}


DNA img_iter::getDNA() const {
	DNA d{polygons.size(), polygons.front().getPolygon().size()};
	for (auto it = polygons.cbegin(); it != polygons.cend(); ++it)
		d.add((*it).getPolygon(), (*it).getColor(), (*it).getAlpha());
	return d;
}


const Canvas& img_iter::getCanvas() const {
	return canvas;
}


void img_iter::drawPolygons() {
	static Color bg{255, 255, 255};
	canvas.clear(bg);
	for (auto it = polygons.cbegin(); it != polygons.cend(); ++it) {
		canvas.setColor((*it).getColor());
		canvas.setAlpha((*it).getAlpha());
		canvas.fill((*it).getPolygon());
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
			accuracy += blockAcc[i][j];
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


int img_iter::countBlocks(const Rectangle& rect) {
	const int iLo = rect.x0 / blockSize;
	const int iHi = rect.x1 % blockSize == 0 ? rect.x1 / blockSize : rect.x1 / blockSize + 1;
	const int jLo = rect.y0 / blockSize;
	const int jHi = rect.y1 % blockSize == 0 ? rect.y1 / blockSize : rect.y1 / blockSize + 1;
	return (iHi - iLo + 1) * (jHi - jLo + 1);
}


void img_iter::recalcAccuracy(const Rectangle& rect) {
	const int iLo = rect.x0 / blockSize;
	const int iHi = rect.x1 % blockSize == 0 ? rect.x1 / blockSize : rect.x1 / blockSize + 1;
	const int jLo = rect.y0 / blockSize;
	const int jHi = rect.y1 % blockSize == 0 ? rect.y1 / blockSize : rect.y1 / blockSize + 1;
	for (int i = iLo; i < iHi; ++i) {
		for (int j = jLo; j < jHi; ++j)
			blockAcc[i][j] = blockAccuracy(i, j);
	}
}
