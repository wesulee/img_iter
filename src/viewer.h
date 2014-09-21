#pragma once

#include "canvas.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <string>


class Viewer {
public:
	Viewer(const Image&, const Canvas&);
	~Viewer();
	void draw(void);
	void processEvents(void);
	bool opened(void) const;
	bool hasError(void) const;
private:
	void redraw(void);
	void logError(const std::string&);
	SDL_Surface* toSurface(const Image&);
	void setPixel(SDL_Surface* const, const int, const int, const Color&);
	void updateIterSurface(void);
	static constexpr int paddingLR = 5;		// padding of left and right side of window
	static constexpr int paddingTB = 5;		// padding of top and bottom of window
	static constexpr int paddingImg = 5;	// padding between images
	const Canvas& canvas;
	std::ostream* log = &std::cerr;
	SDL_Window* win = nullptr;
	SDL_Surface* screen = nullptr;
	SDL_Surface* imgOrig = nullptr;
	SDL_Surface* imgIter = nullptr;
	const int imgWidth;
	const int imgHeight;
	const int width;	// window dimensions
	const int height;
	SDL_Rect rectOrig;	// draw dest
	SDL_Rect rectIter;
	SDL_Event e;
	bool error = false;
	bool quit = false;
};
