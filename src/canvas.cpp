#include "canvas.h"


Canvas::Canvas(const int w, const int h)
: WIDTH(w), HEIGHT(h) {
	colors = new Color*[h];
	for (int i = 0; i < h; ++i)
		colors[i] = new Color[w];

}


Canvas::Canvas(const Image& img)
: Canvas(img.width(), img.height()) {
	for (int x = 0; x < WIDTH; ++x) {
		for (int y = 0; y < HEIGHT; ++y) {
			setColor(img.get(x, y));
			drawPoint(x, y);
		}
	}
}


Canvas::~Canvas() {
	for (int i = 0; i < HEIGHT; ++i) {
		delete[] colors[i];
		colors[i] = nullptr;
	}
	delete[] colors;
	colors = nullptr;
}


void Canvas::setColor(const Color& c) {
	brushColor = c;
}


void Canvas::setAlpha(const float a) {
	if (a >= 0 && a <= 1)
		alpha = a;
}


Image Canvas::getImage() const {
	Image img{WIDTH, HEIGHT};
	for (int x = 0; x < WIDTH; ++x) {
		for (int y = 0; y < HEIGHT; ++y)
			img.set(x, y, getPoint(x, y));
	}
	return img;
}


Color& Canvas::getPoint(const int x, const int y) {
	return colors[y][x];
}


Color Canvas::getPoint(const int x, const int y) const {
	return colors[y][x];
}


void Canvas::setPoint(const int x, const int y, const Color& c) {
	Color& color = getPoint(x, y);
	color = c;
}


void Canvas::drawPoint(const int x, const int y) {
	if (x < WIDTH && y < HEIGHT)
		getPoint(x, y).blend(brushColor, alpha);
}


void Canvas::drawLine(const int x0, const int y0, const int x1, const int y1) {
	const int dx = std::abs(x1 - x0);
	const int dy = std::abs(y1 - y0);
	const int sx = x0 < x1 ? 1 : -1;
	const int sy = y0 < y1 ? 1 : -1;
	int err = dx - dy;
	int e2;
	int x = x0;
	int y = y0;
	while (true) {
		drawPoint(x, y);
		if (x == x1 && y == y1)
			break;
		e2 = err * 2;
		if (e2 > -dy) {
			err -= dy;
			x += sx;
		}
		if (e2 < dx) {
			err += dx;
			y += sy;
		}
	}
}


// like drawLine() but doesn't draw last pixel (for drawing connected lines)
void Canvas::drawLine2(const int x0, const int y0, const int x1, const int y1) {
	const int dx = std::abs(x1 - x0);
	const int dy = std::abs(y1 - y0);
	const int sx = x0 < x1 ? 1 : -1;
	const int sy = y0 < y1 ? 1 : -1;
	int err = dx - dy;
	int e2;
	int x = x0;
	int y = y0;
	while (true) {
		if (x == x1 && y == y1)
			break;

		drawPoint(x, y);
		e2 = err * 2;
		if (e2 > -dy) {
			err -= dy;
			x += sx;
		}
		if (e2 < dx) {
			err += dx;
			y += sy;
		}
	}
}


// draw a horizontal line (for filling polygons)
void Canvas::drawLineH(const int x0, const int y, const int x1) {
	for (int x = x0; x <= x1; ++x)
		drawPoint(x, y);
}


void Canvas::drawRect(const int x, const int y, const int w, const int h) {
	int cx = x;
	int cy = y;
	// top
	for (; cx < x + w; ++cx)
		drawPoint(cx, cy);
	// right
	for (; cy < y + h; ++cy)
		drawPoint(cx, cy);
	// bottom
	for (; cx > x; --cx)
		drawPoint(cx, cy);
	// left
	for (; cy > y; --cy)
		drawPoint(cx, cy);
}


void Canvas::fillRect(const int x, const int y, const int w, const int h) {
	for (int cx = x; cx < x + w; ++cx) {
		for (int cy = y; cy < y + h; ++cy)
			drawPoint(cx, cy);
	}
}


void Canvas::draw(const Polygon& p) {
	auto& vert = p.vertices();
	auto it = vert.cbegin();
	int prevX = (*it).x;
	int prevY = (*it).y;
	for (++it; it != vert.cend(); ++it) {
		drawLine2(prevX, prevY, (*it).x, (*it).y);
		prevX = (*it).x;
		prevY = (*it).y;
	}
	// draw closing line
	drawLine2(prevX, prevY, vert.front().x, vert.front().y);
}


void Canvas::draw(const Image& img) {
	const int xLim = img.width() > WIDTH ? WIDTH : img.width();
	const int yLim = img.height() > HEIGHT ? HEIGHT : img.height();
	Color oldColor{brushColor};
	for (int x = 0; x < xLim; ++x) {
		for (int y = 0; y < yLim; ++y) {
			setColor(img.get(x, y));
			drawPoint(x, y);
		}
	}
	setColor(oldColor);
}


void Canvas::fill(const Polygon& p) {
	const auto lines = p.fillDetails();
	int state;
	int x = 0;	// default value
	for (auto it = lines.cbegin(); it != lines.cend(); ++it) {
		state = 0;
		for (auto xit = (*it).xList.cbegin(); xit != (*it).xList.cend(); ++xit) {
			switch (state) {
			case 0:
				x = *xit;
				break;
			case 1:
				drawLineH(x, (*it).y, *xit);
				break;
			default:
				break;
			}
			state = (state + 1) % 2;
		}
	}
}


void Canvas::clear() {
	for (int x = 0; x < WIDTH; ++x) {
		for (int y = 0; y < HEIGHT; ++y)
			drawPoint(x, y);
	}
}


void Canvas::clear(const Color& c) {
	for (int x = 0; x < WIDTH; ++x) {
		for (int y = 0; y < HEIGHT; ++y)
			setPoint(x, y, c);
	}
}


int Canvas::width() const {
	return WIDTH;
}


int Canvas::height() const {
	return HEIGHT;
}
