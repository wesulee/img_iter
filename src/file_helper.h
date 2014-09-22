#pragma once

#include "color.h"
#include "dna.h"
#include "image.h"
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <map>
#include <sstream>
#include <string>


// work around error: 'to_string' is not a member of 'std'
namespace patch {
	template<typename T>
	std::string to_string(const T& n) {
		std::ostringstream stm;
		stm << n;
		return stm.str();
	}
}


enum class ImageFormat {NONE, PPM};
std::string ImageFormatToExtension(const ImageFormat);
ImageFormat extensionToImageFormat(const std::string&);


class ImageReader {
public:
	ImageReader() = default;
	ImageReader(const ImageReader&) = delete;
	~ImageReader() = default;
	ImageReader& operator=(const ImageReader&) = delete;

	bool read(const std::string&);
	Image getImage(void) const;
	const std::string& getPath(void) const;
	const std::string& getError(void) const;
	void clear(void);
protected:
	Image img;
	std::string path;
	std::string error;
private:
	ImageFormat getFormat(const std::string&);
};


class ImageWriter {
public:
	ImageWriter() {}
	~ImageWriter() {}
	bool write(const Image&, const std::string&, const ImageFormat);
};


// DNA file format: VERTEX_COUNT POLYGON_COUNT R G B A X0 Y0 X1 Y1 ... XN YN ... R G B A X0 Y0 X1 Y1 ... XN YN ...
DNA readDNA(const std::string&, std::string&);
bool writeDNA(const DNA&, const std::string&);


namespace FileHelper {


bool isUInt(const std::string&);
bool isSimpleFloat(const std::string&);


class FileReader {
public:
	FileReader() = default;
	~FileReader() = default;
	bool open(const std::string&, std::ios_base::openmode);
	char readChar(void);
	std::string readWord(void);
	bool eof(void) const;
private:
	void checkEOF(void);

	std::ifstream f;
	std::string path;
	bool endOfFile = true;
	int next;
};


class FileWriter {
public:
	FileWriter() = default;
	~FileWriter() = default;
	bool open(const std::string&, std::ios_base::openmode);
	void put(const char);
	void write(const std::string&);
	void write(const char*, std::streamsize);
	bool good(void) const;
	template <class T> FileWriter& operator<<(const T&);
private:
	std::ofstream f;
	std::string path;
	bool isGood = false;
};


class ImgReaderBase {
	friend ImageReader;
public:
	ImgReaderBase(const ImgReaderBase&) = delete;
	ImgReaderBase& operator=(const ImgReaderBase&) = delete;
	virtual ~ImgReaderBase() {}

	virtual bool read(const std::string&) = 0;
protected:
	ImgReaderBase() {}

	Image img;
	std::string error;
};


class ImgWriterBase {
public:
	virtual ~ImgWriterBase() {}
	virtual bool write(const Image&, const std::string&) = 0;
protected:
	ImgWriterBase() {}
};


// used to read a generated ppm file
class readPPM : public ImgReaderBase {
public:
	readPPM() = default;
	readPPM(const readPPM&) = delete;
	~readPPM() = default;
	bool read(const std::string&) override;
private:
	static constexpr int limitMB = 10;
};


class writePPM : public ImgWriterBase {
public:
	writePPM() = default;
	~writePPM() = default;
	bool write(const Image&, const std::string&) override;
};


}	// namespace FileHelper
