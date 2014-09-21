#include "image.h"


Image::Image(const int w, const int h) {
	allocate(w, h);
}


Image::Image(const Image& img) : Image(img.width(), img.height()) {
	copy(img);
}


Image::~Image() {
	clear();
}


Image& Image::operator=(const Image& img) {
	if (WIDTH != img.width() || HEIGHT != img.height()) {
		clear();
		allocate(img.width(), img.height());
	}
	copy(img);
	return *this;
}


void Image::resize(const int w, const int h) {
	if (w == WIDTH && h == HEIGHT)
		return;

	clear();
	allocate(w, h);
}


Color Image::get(const int x, const int y) const {
	if (x < WIDTH && y < HEIGHT)
		return getRef(x, y);
	else
		throw std::out_of_range("");
}


void Image::set(const int x, const int y, const Color& c) {
	if (x < WIDTH && y < HEIGHT) {
		getRef(x, y) = c;
	}
	else
		throw std::out_of_range("");
}


int Image::width() const {
	return WIDTH;
}


int Image::height() const {
	return HEIGHT;
}


bool Image::empty() const {
	return data == nullptr;
}


void Image::clear() {
	if (data != nullptr)
		deallocate(data, HEIGHT);

	data = nullptr;
	WIDTH = 0;
	HEIGHT = 0;
}


void Image::allocate(const int w, const int h) {
	data = getAllocation(w, h);
	WIDTH = w;
	HEIGHT = h;
}


Color** Image::getAllocation(const int width, const int height) {
	Color** ptr = new Color*[height];
	for (int i = 0; i < height; ++i)
		ptr[i] = new Color[width];
	return ptr;
}


void Image::deallocate(Color** ptr, const int height) {
	for (int h = 0; h < height; ++h) {
		delete[] ptr[h];
		ptr[h] = nullptr;
	}
	delete[] ptr;
}


void Image::copy(const Image& img) {
	for (int x = 0; x < img.width(); ++x) {
		for (int y = 0; y < img.height(); ++y) {
			getRef(x, y) = img.get(x, y);
		}
	}
}


Color& Image::getRef(const int x, const int y) {
	return data[y][x];
}



const Color& Image::getRef(const int x, const int y) const {
	return data[y][x];
}
