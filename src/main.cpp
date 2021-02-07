#include <iostream>

#include "lex/lexer.h"
#include "util/source_file.h"

static void usage(int argc, const char** argv)
{
	std::cout << "usage: " << argv[0] << " <file>" << std::endl;
}

int main(int argc, const char** argv)
{
	if(argc == 2)
	{
		arc::source_file input(argv[1]);
		if(input.exists())
		{
			arc::lexer lexer(input);
			lexer.lex();
		}
		else
		{
			std::cout << "'" << argv[1] << "' not found" << std::endl;
		}
	}
	else
	{
		usage(argc, argv);
	}
}
