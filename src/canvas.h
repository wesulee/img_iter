#pragma once

#include "color.h"
#include "image.h"
#include "polygon.h"
#include <cassert>
#include <cmath>


class Canvas {
public:
	Canvas(const int, const int);
	Canvas(const Image&);
	~Canvas();
	void setColor(const Color&);
	void setAlpha(const float);
	void drawPoint(const int, const int);
	void drawLine(const int, const int, const int, const int);
	void drawRect(const int, const int, const int, const int);
	void drawEllipse(const int, const int, const int);
	void fillEllipse(const int, const int, const int);
	void draw(const Polygon&);
	void draw(const Image&);
	void fill(const Rectangle&);
	void fill(const Polygon&);
	void fill(const Polygon&, const Rectangle&);
	void clear(void);
	void clear(const Color&);
	Image getImage(void) const;
	Color getPoint(const int, const int) const;
	int width(void) const;
	int height(void) const;
private:
	void setPoint(const int, const int, const Color&);
	Color& getPoint(const int, const int);
	void drawLine2(const int, const int, const int, const int);
	void drawLineH(const int, const int, const int);
	void drawLineV(const int, const int, const int);

	Color brushColor;
	float alpha = 1.0;
	const int WIDTH;
	const int HEIGHT;
	Color** colors;
};
