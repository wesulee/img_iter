#include "arg_parser.h"


img_iter_saver::img_iter_saver(const std::string& imgPath, const ImageFormat f,
	const img_iter& ii, SaveOption so, int num, std::ostream& os)
: ii(ii), so(so), optNum(num), imgPath(imgPath), saveFormat(f),
  saveExt(ImageFormatToExtension(saveFormat)), os(os) {
}


void img_iter_saver::update() {
	if (check())
		save();
}


void img_iter_saver::save() {
	if (last == ii.improvements())
		return;
	// print status
	os << "Iter: " << std::setw(6) << ii.iterations()
	   << "\tImp: " << std::setw(6) << ii.improvements()
	   << "\tFit: " << ii.fitness() * 100
	   << "\tTime: " << std::setw(6) << ii.runtime() << " s"
	   << std::endl;
	last = ii.improvements();
	iw.write(ii.getImage(), saveImgPath(), saveFormat);
	writeDNA(ii.getDNA(), saveDNAPath());
}


bool img_iter_saver::check() const {
	const int improvements = ii.improvements();
	switch (so) {
	case SaveOption::ITERATIONS:
		if ((ii.iterations() % optNum == 0) && (last != improvements))
			return true;
		break;
	case SaveOption::IMPROVEMENTS:
		if ((improvements % optNum == 0) && (last != improvements))
			return true;
		break;
	default:
		break;
	}
	return false;
}


std::string img_iter_saver::saveImgPath() const {
	std::string ret{imgPath};
	ret += '.';
	std::string tmp{patch::to_string(ii.iterations())};
	if (tmp.size() < padSize) {
		tmp.insert(0, padSize - tmp.size(), '0');
	}
	ret += tmp;
	ret += '.';
	ret += saveExt;
	return ret;
}


std::string img_iter_saver::saveDNAPath() const {
	std::string ret{imgPath};
	ret += '.';
	std::string tmp{patch::to_string(ii.iterations())};
	if (tmp.size() < padSize) {
		tmp.insert(0, padSize - tmp.size(), '0');
	}
	ret += tmp;
	ret += ".txt";
	return ret;
}


arg_parser::arg_parser(const int argc, char** argv) {
	// populate argument descriptions
	std::list<argument_data> arg_data;
	argument_data empty_arg_data;
	argument_data tmp;

	tmp.command = 'i';
	tmp.arguments.push_back("image_path");
	tmp.description = "begin iteration from image";
	arg_data.push_back(tmp);
	tmp = empty_arg_data;

	tmp.command = 'd';
	tmp.arguments.push_back("dna_path");
	tmp.description = "resume iteration from DNA";
	arg_data.push_back(tmp);
	tmp = empty_arg_data;

	tmp.command = 'p';
	tmp.arguments.push_back("number");
	tmp.description = "polygon count";
	arg_data.push_back(tmp);
	tmp = empty_arg_data;

	tmp.command = 'v';
	tmp.arguments.push_back("number");
	tmp.description = "vertex count";
	arg_data.push_back(tmp);
	tmp = empty_arg_data;

	tmp.command = 's';
	tmp.arguments.push_back("number");
	tmp.arguments.push_back("'iter' OR 'imp'");
	tmp.description = "save every <number> iterations or improvements";
	arg_data.push_back(tmp);
	tmp = empty_arg_data;

	tmp.command = "console";
	tmp.description = "display console only";
	arg_data.push_back(tmp);
	tmp = empty_arg_data;

	tmp.command = "log";
	tmp.arguments.push_back("file_path");
	tmp.description = "save console output to a file";
	arg_data.push_back(tmp);
	tmp = empty_arg_data;

	if (argc < 2) {
		helpMenu(arg_data);
		return;
	}

	// collect input
	std::list<input> in;
	std::string tstr;
	input empty_input;
	input tinput;
	for (int i = 1; i < argc; ++i) {
		tstr = argv[i];
		if (tstr[0] == '-') {	// command given
			if (!tinput.command.empty()) {	// add temp input to in if necessary
				in.push_back(tinput);
				tinput = empty_input;
			}
			// check if command is valid
			std::string command = tstr.substr(1);
			if (validCommand(command, arg_data)) {
				tinput.command = command;
			}
		}
		else {
			if (!tinput.command.empty())
				tinput.arguments.push_back(tstr);
		}
	}
	if (!tinput.command.empty()) {
		in.push_back(tinput);
		tinput = empty_input;
	}

	// process input
	for (auto it = in.cbegin(); it != in.cend(); ++it) {
		if ((*it).command == "i") {
			if ((*it).arguments.size() != 1) {
				std::cout << "Expecting one argument for -i" << std::endl;
				return;
			}
			imgPath = (*it).arguments.front();
		}
		else if ((*it).command == "d") {
			if ((*it).arguments.size() != 1) {
				std::cout << "Expecting one argument for -d" << std::endl;
				continue;
			}
			dnaPath = (*it).arguments.front();
		}
		else if ((*it).command == "p") {
			if ((*it).arguments.size() != 1) {
				std::cout << "Expecting one argument for -p" << std::endl;
				continue;
			}
			if (FileHelper::isUInt((*it).arguments.front())) {
				polyCount = std::atoi((*it).arguments.front().c_str());
			}
			else {
				std::cout << "Invalid number for -p" << std::endl;
			}
		}
		else if ((*it).command == "v") {
			if ((*it).arguments.size() != 1) {
				std::cout << "Expecting one argument for -v" << std::endl;
				continue;
			}
			if (FileHelper::isUInt((*it).arguments.front())) {
				vertCount = std::atoi((*it).arguments.front().c_str());
			}
			else {
				std::cout << "Invalid number for -v" << std::endl;
			}
		}
		else if ((*it).command == "s") {
			if ((*it).arguments.size() != 2) {
				std::cout << "Expecting two arguments for -s" << std::endl;
				continue;
			}
			auto it2 = (*it).arguments.cbegin();
			if (!FileHelper::isUInt(*it2)) {
				std::cout << "Invalid number for -s" << std::endl;
				continue;
			}
			save_option_number = std::atoi((*it2).c_str());
			++it2;
			if ((*it2) == "iter") {
				save_option = SaveOption::ITERATIONS;
			}
			else if ((*it2) == "imp") {
				save_option = SaveOption::IMPROVEMENTS;
			}
			else {
				std::cout << "Invalid second argument for -s" << std::endl;
			}
		}
		else if ((*it).command == "console") {
			program_mode = ProgramMode::CONSOLE;
		}
		else if ((*it).command == "log") {
			if ((*it).arguments.size() != 1) {
				std::cout << "Expecting one argument for -log" << std::endl;
				continue;
			}
			logPath = (*it).arguments.front();
		}
	}

	valid = !imgPath.empty();
	if (!valid) {
		std::cout << "Missing argument -i" << std::endl;
	}
}


void arg_parser::execute() {
	if (!valid)
		return;

	std::streambuf* buf = std::cout.rdbuf();
	std::ofstream logFile;
	ImageReader ir;
	img_iter* ii = nullptr;
	Image orig;
	// log file setup
	if (!logPath.empty()) {
		logFile.open(logPath, std::ofstream::out | std::ofstream::app);
		if (logFile)
			buf = logFile.rdbuf();
		else
			std::cout << "Error opening log file" << std::endl;
	}
	std::ostream saveStream{buf};

	ir.read(imgPath);
	orig = ir.getImage();
	if (orig.empty()) {
		std::cout << "Error reading image: " << ir.getError() << std::endl;
		return;
	}

	if (dnaPath.empty()) {
		ii = new img_iter(orig, polyCount, vertCount);
	}
	else {
		std::string dnaReadError;
		DNA dna{readDNA(dnaPath, dnaReadError)};
		if (!dnaReadError.empty()) {
			std::cout << "Error reading DNA: " << dnaReadError << std::endl;
			return;
		}
		ii = new img_iter(orig, dna);
	}

	// run
	img_iter_saver iis{imgPath, ImageFormat::PPM, *ii, save_option, save_option_number, saveStream};
	if (program_mode == ProgramMode::VIEWER) {
		Viewer viewer{orig, ii->bestImage()};
		if (viewer.hasError())
			return;

		int imp = 0;
		while (viewer.opened()) {
			ii->iterate();
			viewer.processEvents();
			iis.update();
			if (ii->improvements() != imp) {
				imp = ii->improvements();
				viewer.draw();
			}
		}
	}
	else if (program_mode == ProgramMode::CONSOLE) {
		if (save_option == SaveOption::ITERATIONS) {
			while (true) {
				ii->iterate();
				iis.update();
			}
		}
		else if (save_option == SaveOption::IMPROVEMENTS) {
			while (true) {
				ii->improve();
				iis.update();
			}
		}
	}

	delete ii;
}


void arg_parser::helpMenu(const std::list<argument_data>& args) {
	std::cout << "Usage: -command <arguments>" << std::endl;
	for (auto it = args.cbegin(); it != args.cend(); ++it) {
		std::cout << "\t-" << (*it).command;
		if (!(*it).arguments.empty()) {
			for (auto it2 = (*it).arguments.cbegin(); it2 != (*it).arguments.cend(); ++it2) {
				std::cout << " <" << (*it2) << '>';
			}
		}
		std::cout << std::endl;
		std::cout << "\t\t" << (*it).description << std::endl;
	}
}


bool arg_parser::validCommand(const std::string& command, const std::list<argument_data>& args) {
	for (auto it = args.cbegin(); it != args.cend(); ++it) {
		if ((*it).command == command)
			return true;
	}
	return false;
}
