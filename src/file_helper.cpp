#include "file_helper.h"


std::string ImageFormatToExtension(const ImageFormat f) {
	switch (f) {
	case ImageFormat::PPM:
		return "ppm";
	case ImageFormat::NONE:
	default:
		return std::string();
	}
}


ImageFormat extensionToImageFormat(const std::string& extension) {
	std::string ext{extension};
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
	if (ext == "ppm")
		return ImageFormat::PPM;
	else
		return ImageFormat::NONE;
}


bool ImageReader::read(const std::string& fpath) {
	using namespace FileHelper;
	path = fpath;
	img.clear();
	ImgReaderBase* reader = nullptr;
	const ImageFormat format = getFormat(path);

	switch (format) {
	case ImageFormat::NONE:
		error = "missing file extension";
		return false;
	case ImageFormat::PPM:
		reader = new readPPM;
		break;
	}

	bool ret = false;
	if (reader != nullptr) {
		ret = reader->read(path);
		error = reader->error;
		if (ret)
			img = reader->img;
		delete reader;
	}
	return ret;
}


Image ImageReader::getImage() const {
	return img;
}


const std::string& ImageReader::getPath() const {
	return path;
}


const std::string& ImageReader::getError() const {
	return error;
}


void ImageReader::clear() {
	path.clear();
	error.clear();
	img.clear();
}


ImageFormat ImageReader::getFormat(const std::string& path) {
	std::string::size_type extIndex = path.rfind('.');
	if (extIndex == std::string::npos)
		return ImageFormat::NONE;

	std::string extension{path.substr(extIndex + 1)};
	return extensionToImageFormat(extension);
}


bool ImageWriter::write(const Image& img, const std::string& path, ImageFormat format) {
	using namespace FileHelper;
	ImgWriterBase* writer = nullptr;
	switch (format) {
	case ImageFormat::NONE:
		break;
	case ImageFormat::PPM:
		writer = new writePPM();
		break;
	}

	if (writer == nullptr)
		return false;
	else {
		bool ret = writer->write(img, path);
		delete writer;
		return ret;
	}
}


bool FileHelper::isUInt(const std::string& str) {
	if (str.empty())
		return false;
	for (auto it = str.cbegin(); it != str.cend(); ++it) {
		if (!std::isdigit(*it))
			return false;
	}
	return true;
}


// true if base-10 float value without e
bool FileHelper::isSimpleFloat(const std::string& str) {
	if (str.empty())
		return false;
	bool period = false;
	std::size_t i = 0;
	// check if begins with + or -
	if (!std::isdigit(str[0])) {
		switch(str[0]) {
		case '+':
		case '-':
			break;
		default:
			return false;
		}
		++i;
	}
	for (; i < str.size(); ++i) {
		if (!std::isdigit(str[i])) {
			switch (str[i]) {
			case '.':
				if (period) {
					// a period has been found before, so invalid
					return false;
				}
				period = true;
				break;
			default:
				return false;
			}
		}
	}
	return true;
}



using namespace FileHelper;


bool FileReader::open(const std::string& fp, std::ios_base::openmode mode = std::ios_base::in) {
	path = fp;
	endOfFile = false;
	f.open(path, mode);
	if (f.is_open()) {
		checkEOF();
		return true;
	}
	else {
		endOfFile = true;
		return false;
	}
}


char FileReader::readChar() {
	if (!endOfFile) {
		checkEOF();
	}
	return static_cast<char>(next);
}


std::string FileReader::readWord() {
	std::string str;
	if (endOfFile)
		return str;

	while (!endOfFile) {
		if (isspace(next)) {
			if (!str.empty())
				break;
		}
		else {
			str += next;
		}
		checkEOF();
	}
	return str;
}


bool FileReader::eof() const {
	return endOfFile;
}


void FileReader::checkEOF() {
	next = f.get();
	if (next == EOF) {
		endOfFile = true;
		if (!f.eof()) {
			throw std::ios_base::failure("read error");
		}
	}
}


bool FileWriter::open(const std::string& fp, std::ios_base::openmode mode = std::ios_base::out) {
	path = fp;
	f.open(path, mode);
	return f.is_open();
}


void FileWriter::put(const char c) {
	(*this) << c;
}


void FileWriter::write(const std::string& str) {
	(*this) << str;
}


void FileWriter::write(const char* s, std::streamsize n) {
	f.write(s, n);
}


bool FileWriter::good() const {
	return f.good();
}


template <class T>
FileWriter& FileWriter::operator<<(const T& a) {
	f << a;
	return *this;
}


bool readPPM::read(const std::string& path) {
	std::ifstream f{path, std::ifstream::in | std::ifstream::binary};
	if (!f) {
		error = "cannot open file";
		return false;
	}
	auto readChar = [&f] () {return static_cast<char>(f.get());};
	auto digitToInt = [] (char c) {return c - '0';};
	char c = 0;	// character read
	int width;
	int height;
	int maxColor;
	// read header data
	// determine type
	std::string type;
	while (type.size() < 2) {
		c = readChar();
		type += c;
	}
	if (type != std::string("P6")) {
		error = "unknown format";
		return false;
	}

	// get width;
	// continue to first non-whitespace character
	while (f && isspace(c = readChar()))
		;
	if (!isdigit(c)) {
		error = "invalid data";
		return false;
	}
	width = digitToInt(c);
	while (isdigit(c = readChar()))
		width = width * 10 + digitToInt(c);
	// continue to non-whitespace
	while (isspace(c = readChar()))
		;
	if (!isdigit(c)) {
		error = "invalid data";
		return false;
	}
	height = digitToInt(c);
	while (isdigit(c = readChar()))
		height = height * 10 + digitToInt(c);

	// assuming 3 bytes per pixel
	if ((static_cast<float>(width * height * 3) / 1000 / 1000) > limitMB) {
		error = "image too large";
		return false;
	}

	// get max color value
	// continue to non-whitespace
	while (isspace(c = readChar()))
		;
	if (!isdigit(c)) {
		error = "invalid data";
		return false;
	}
	maxColor = digitToInt(c);
	while (isdigit(c = readChar()))
		maxColor = maxColor * 10 + digitToInt(c);
	if (maxColor != 255) {
		error = "max color value is not 255";
		return false;
	}

	img.resize(width, height);

	// read pixel data
	Color color;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			color.R = static_cast<Color::ColorChannel>(readChar());
			color.G = static_cast<Color::ColorChannel>(readChar());
			color.B = static_cast<Color::ColorChannel>(readChar());
			img.set(x, y, color);
		}
	}

	f.close();
	return true;
}


bool writePPM::write(const Image& img, const std::string& path) {
	std::ofstream f{path, std::ofstream::out | std::ofstream::binary};
	if (!f)
		return false;
	// write header
	std::string header = std::string("P6\n") + patch::to_string(img.width()) + ' '
		+ patch::to_string(img.height()) + std::string("\n255\n");
	f.write(header.c_str(), header.size());
	if (f.fail())
		return false;
	// write data
	unsigned char pxData[3];	// pixel data
	for (int y = 0; y < img.height(); ++y) {
		for (int x = 0; x < img.width(); ++x) {
			const Color c{img.get(x, y)};
			pxData[0] = static_cast<unsigned char>(c.R);
			pxData[1] = static_cast<unsigned char>(c.G);
			pxData[2] = static_cast<unsigned char>(c.B);
			f.write(reinterpret_cast<char*>(pxData), 3);
			if (f.fail())
				return false;
		}
	}
	f.close();
	return true;
}


DNA readDNA(const std::string& path, std::string& error) {
	DNA dnaError;	// return this if there was an error or invalid format
	DNA d;
	FileReader r;
	if (!r.open(path)) {
		error = "unable to open file";
		return dnaError;
	}
	if (r.eof()) {
		error = "empty file";
		return dnaError;
	}

	int value;
	std::string word;
	// vertex count
	word = r.readWord();
	if (!isUInt(word)) {
		error = "invalid vertex count";
		return dnaError;
	}
	value = std::atoi(word.c_str());
	if (value < 3) {
		error = "invalid vertex count";
		return dnaError;
	}
	d.vertCount = value;
	// polygon count
	word = r.readWord();
	if (!isUInt(word)) {
		error = "invalid polygon count";
		return dnaError;
	}
	value = std::atoi(word.c_str());
	if (value < 1) {
		error = "invalid polygon count";
		return dnaError;
	}
	d.polyCount = value;
	// read polygons
	Color color;
	float alpha;
	Polygon::Point pp;
	Polygon p;
	for (std::size_t i = 0; i < d.polyCount; ++i) {
		// R
		word = r.readWord();
		if (!isUInt(word)) {
			error = "invalid color R value";
			return dnaError;
		}
		value = std::atoi(word.c_str());
		if (value < Color::minColorChannel || value > Color::maxColorChannel) {
			error = "invalid color R value";
			return dnaError;
		}
		color.R = static_cast<Color::ColorChannel>(value);
		// G
		word = r.readWord();
		if (!isUInt(word)) {
			error = "invalid color G value";
			return dnaError;
		}
		value = std::atoi(word.c_str());
		if (value < Color::minColorChannel || value > Color::maxColorChannel) {
			error = "invalid color G value";
			return dnaError;
		}
		color.G = static_cast<Color::ColorChannel>(value);
		// B
		word = r.readWord();
		if (!isUInt(word)) {
			error = "invalid color B value";
			return dnaError;
		}
		value = std::atoi(word.c_str());
		if (value < Color::minColorChannel || value > Color::maxColorChannel) {
			error = "invalid color B value";
			return dnaError;
		}
		color.B = static_cast<Color::ColorChannel>(value);
		// A
		word = r.readWord();
		if (!isSimpleFloat(word)) {
			error = "invalid alpha value";
			return dnaError;
		}
		alpha = static_cast<float>(std::atof(word.c_str()));
		if (alpha < 0 || alpha > 1) {
			error = "invalid alpha value";
			return dnaError;
		}
		// vertices
		for (std::size_t j = 0; j < d.vertCount; ++j) {
			// X
			word = r.readWord();
			if (!isUInt(word)) {
				error = "invalid vertex x value";
				return dnaError;
			}
			value = std::atoi(word.c_str());
			pp.x = value;
			// Y
			word = r.readWord();
			if (!isUInt(word)) {
				error = "invalid vertex y value";
				return dnaError;
			}
			value = std::atoi(word.c_str());
			pp.y = value;
			p.add(pp);
		}
		d.add(p, color, alpha);
		p.clear();
	}
	return d;
}


// returns true if successfully wrote DNA data to file
bool writeDNA(const DNA& d, const std::string& path) {
	std::ofstream f{path};
	if (!f)
		return false;

	f << d.vertCount << ' ' << d.polyCount;
	for (auto it = d.data.cbegin(); it != d.data.cend(); ++it) {
		f << ' ' << static_cast<int>((*it).color.R);
		f << ' ' << static_cast<int>((*it).color.G);
		f << ' ' << static_cast<int>((*it).color.B);
		f << ' ' << (*it).alpha;
		for (std::size_t i = 0; i < (*it).v.size(); ++i) {
			f << ' ' << (*it).v[i].x;
			f << ' ' << (*it).v[i].y;
		}
	}

	f.close();
	return true;
}
