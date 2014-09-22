#include "viewer.h"


Viewer::Viewer(const Image& img, const Canvas& can)
: canvas(can), imgWidth(img.width()), imgHeight(img.height()),
  width((paddingLR + imgWidth)*2 + paddingImg),
  height(paddingTB*2 + imgHeight), rectOrig({paddingLR, paddingTB, imgWidth, imgHeight}),
  rectIter({rectOrig.x + imgWidth + paddingImg, rectOrig.y, rectOrig.w, rectOrig.h}) {
	// initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		logError("SDL_Init");
		return;
	}
	win = SDL_CreateWindow(
		"img_iter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height, SDL_WINDOW_SHOWN
	);
	if (win == nullptr) {
		logError("SDL_CreateWindow");
		return;
	}
	screen = SDL_GetWindowSurface(win);
	imgOrig = toSurface(img);
	if (imgOrig == nullptr)
		return;
	imgIter = toSurface(canvas.getImage());
	if (imgIter == nullptr)
		return;

	redraw();
}


Viewer::~Viewer() {
	SDL_FreeSurface(imgOrig);
	imgOrig = nullptr;
	SDL_FreeSurface(imgIter);
	imgIter = nullptr;

	SDL_DestroyWindow(win);
	win = nullptr;

	SDL_Quit();
}


void Viewer::draw() {
	updateIterSurface();
	SDL_BlitSurface(imgIter, NULL, screen, &rectIter);
	SDL_UpdateWindowSurface(win);
}


void Viewer::processEvents() {
	while(SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_QUIT) {
			quit = true;
		}
	}
}


bool Viewer::opened() const {
	return !quit;
}


bool Viewer::hasError() const {
	return error;
}


void Viewer::redraw() {
	SDL_FillRect(screen, NULL, 0);	// black background
	SDL_BlitSurface(imgOrig, NULL, screen, &rectOrig);
	draw();
}


void Viewer::logError(const std::string& msg) {
	if (log != nullptr)
		*log << msg << " error: " << SDL_GetError() << std::endl;
	error = true;
}


SDL_Surface* Viewer::toSurface(const Image& img) {
	SDL_Surface* surface = SDL_CreateRGBSurface(
		SDL_SWSURFACE, img.width(), img.height(), 32,
		screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, 0
	);
	if (surface == nullptr) {
		logError("SDL_CreateRGBSurface");
		return surface;
	}

	if (SDL_MUSTLOCK(surface))
		SDL_LockSurface(surface);

	for (int x = 0; x < img.width(); ++x) {
		for (int y = 0; y < img.height(); ++y) {
			setPixel(surface, x, y, img.get(x, y));
		}
	}

	if (SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);
	return surface;
}


void Viewer::setPixel(SDL_Surface* const surface, const int x, const int y, const Color& c) {
	Uint32 *pixels = (Uint32*) surface->pixels;
	pixels[(y * surface->w) + x] = SDL_MapRGB(screen->format, c.R, c.G, c.B);
}


void Viewer::updateIterSurface() {
	for (int x = 0; x < imgIter->w; ++x) {
		for (int y = 0; y < imgIter->h; ++y)
			setPixel(imgIter, x, y, canvas.getPoint(x, y));
	}
}
