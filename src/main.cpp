#include "arg_parser.h"


int main(int argc, char** argv) {
	arg_parser ap{argc, argv};
	ap.execute();

	return 0;
}
