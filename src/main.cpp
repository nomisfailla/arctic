#include <iostream>
#include <stack>

#include "lex/lexer.h"
#include "parse/ast.h"
#include "util/source_file.h"

static void usage(int argc, const char** argv)
{
	std::cout << "usage: " << argv[0] << " <file>" << std::endl;
}

static std::string format_error(const arc::line_exception& ex)
{
	std::string error = "";

	error += "error: " + ex.error + " at " + std::to_string(ex.position.line) + ":" + std::to_string(ex.position.column) + "\n";
	error += std::to_string(ex.position.line) + " | " + ex.file.get_line(ex.position.line) + "\n";

	return error;
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

				for (const auto& t : tokens)
				{
					std::cout << static_cast<int>(t.type) << " @ " << t.position.line << ":" << t.position.column << std::endl;
				}
			}
			catch(const arc::line_exception& ex)
			{
				std::cout << format_error(ex);
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
