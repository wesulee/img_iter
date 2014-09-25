#include "color.h"


Color::Color() : R(defColorChannel), G(defColorChannel), B(defColorChannel) {
}


Color::Color(ColorChannel r, ColorChannel g, ColorChannel b) : R(r), G(g), B(b) {
}


Color::Color(const Color& c) : R(c.R), G(c.G), B(c.B) {
}


Color& Color::operator=(const Color& c) {
	this->R = c.R;
	this->G = c.G;
	this->B = c.B;
	return *this;
}


void Color::blend(const Color& c, const float a) {
	R = blend(R, c.R, a);
	G = blend(G, c.G, a);
	B = blend(B, c.B, a);
}


Color::ColorChannel Color::blend(const ColorChannel cc1, const ColorChannel cc2, const float a) {
	return static_cast<ColorChannel>((cc2 * a) + cc1 * (1.0f - a));
}


// blend background (c1) with foreground (c2) with alpha (a)
Color Color::blend(const Color& c1, const Color& c2, const float a) {
	return Color{blend(c1.R, c2.R, a), blend(c1.G, c2.G, a), blend(c1.B, c2.B, a)};
}
