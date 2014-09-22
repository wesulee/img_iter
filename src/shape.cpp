#include "shape.h"


void ShapeHelper::updateRectBoundsX(Rectangle& rect, const int x) {
	if (x < rect.x0)
		rect.x0 = x;
	else if (x > rect.x1)
		rect.x1 = x;
}


void ShapeHelper::updateRectBoundsY(Rectangle& rect, const int y) {
	if (y < rect.y0)
		rect.y0 = y;
	else if (y > rect.y1)
		rect.y1 = y;
}

Rectangle ShapeHelper::joinRectangles(const Rectangle& rect1, const Rectangle& rect2) {
	Rectangle ret{rect1};
	updateRectBoundsX(ret, rect2.x0);
	updateRectBoundsY(ret, rect2.y0);
	updateRectBoundsX(ret, rect2.x1);
	updateRectBoundsY(ret, rect2.y1);
	return ret;
}
