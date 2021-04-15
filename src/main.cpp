#include <iostream>
#include <stack>

#include "lex/lexer.h"
#include "parse/ast.h"
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

			try
			{
				auto tokens = lexer.lex();
			}
			catch(const arc::line_exception& ex)
			{
				std::cout << "error: " << ex.error << " at " << ex.position.line << ":" << ex.position.column << std::endl;
				std::cout << ex.position.line << " | " << ex.file.get_line(ex.position.line) << std::endl;
			}
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
