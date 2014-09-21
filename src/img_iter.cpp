#include "img_iter.h"


img_iter::img_iter(const Image& img, const int pc, const int vc)
: original(img), canvas(img.width(), img.height()), pm(pc, vc, img.width(),img.height()),
  maxAccuracy(getMaxAccuracy(img)) {
	polygons.reserve(pc);
	for (int i = 0; i < pc; ++i)
		polygons.emplace_back(pm);
	drawPolygons();
	fit = getFitness();
	start = std::chrono::high_resolution_clock::now();
}


img_iter::img_iter(const Image& img, const DNA& d)
: original(img), canvas(img), pm(d.polyCount, d.vertCount, img.width(), img.height()),
  maxAccuracy(getMaxAccuracy(img)) {
	polygons.reserve(d.polyCount);
	for (auto it = d.data.begin(); it != d.data.end(); ++it)
		polygons.emplace_back(pm, *it);
	drawPolygons();
	fit = getFitness();
	start = std::chrono::high_resolution_clock::now();
}


void img_iter::iterate() {
	++iter;
	IterPoly& ip = polygons[pm.randPolyIndex()];
	ip.mutate();
	drawPolygons();
	float mfitness = getFitness();
	if (mfitness < fit)
		ip.undo();
	else {
		++imp;
		fit = mfitness;
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
	for (int x = 0; x < canvas.width(); ++x) {
		for (int y = 0; y < canvas.height(); ++y)
			accuracy += getAccuracy(original.get(x, y), canvas.getPoint(x, y));
	}
	return accuracy / maxAccuracy;
}
