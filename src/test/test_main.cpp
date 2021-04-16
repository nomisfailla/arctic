#include "test_main.h"

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include <vector>

int arc_test_main(int argc, const char** argv)
{
    std::vector<const char*> args;

    // Rebuild arguments.
    // This method would have been invoked with 'arc test' which would make
    // catch try and run the test named 'test', so we will remove that arg
    // at index 1.
    for(int i = 0; i < argc; i++)
    {
        if(i != 1)
        {
            args.push_back(argv[i]);
        }
    }

    return Catch::Session().run(args.size(), args.data());
}
