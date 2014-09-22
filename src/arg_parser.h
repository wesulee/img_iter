#pragma once

#include "file_helper.h"
#include "img_iter.h"
#include "viewer.h"
#include <iomanip>
#include <list>
#include <string>


enum class SaveOption {ITERATIONS, IMPROVEMENTS};
enum class ProgramMode {VIEWER, CONSOLE};


class img_iter_saver {
public:
	img_iter_saver(const std::string&, const ImageFormat, const img_iter&, SaveOption, int);
	~img_iter_saver() = default;
	void update(void);
	void save(void);
private:
	bool check(void) const;
	std::string saveImgPath(void) const;
	std::string saveDNAPath(void) const;
	static constexpr int padSize = 6;
	const img_iter& ii;
	const SaveOption so;
	const int optNum;
	const std::string imgPath;
	const ImageFormat saveFormat;
	const std::string saveExt;
	int last = 0;
	ImageWriter iw;
};


class arg_parser {
	struct argument_data {
		std::string command;
		std::list<std::string> arguments;
		std::string description;
	};
	struct input {
		std::string command;
		std::list<std::string> arguments;
	};
public:
	arg_parser(const int, char**);
	void execute(void);
private:
	void helpMenu(const std::list<argument_data>&);
	static bool validCommand(const std::string&, const std::list<argument_data>&);
	std::string imgPath;
	std::string dnaPath;
	int polyCount = 50;
	int vertCount = 6;
	int save_option_number = 100;
	SaveOption save_option = SaveOption::IMPROVEMENTS;
	ProgramMode program_mode = ProgramMode::VIEWER;
	bool valid = false;
};
