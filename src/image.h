#pragma once

#include "color.h"
#include <stdexcept>


class Image {
public:
	Image() = default;
	Image(const Image&);
	Image(const int, const int);
	~Image();
	Image& operator=(const Image&);

	void resize(const int, const int);
	Color get(const int, const int) const;
	void set(const int, const int, const Color&);
	int width(void) const;
	int height(void) const;
	bool empty(void) const;
	void clear(void);
private:
	void allocate(const int, const int);
	static Color** getAllocation(const int, const int);
	static void deallocate(Color**, const int);
	void copy(const Image&);
	Color& getRef(const int, const int);
	const Color& getRef(const int, const int) const;

	Color** data = nullptr;
	int WIDTH = 0;
	int HEIGHT = 0;
};
