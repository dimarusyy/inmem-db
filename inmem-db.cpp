#include <iostream>

#include "include/property.h"
#include "include/storage.h"
#include "include/struct_index.h"
#include "include/orm_index.h"
#include "include/logger.h"

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

int main(int argc, char** argv)
{
    doctest::Context context;
    context.applyCommandLine(argc, argv);

    auto res = context.run(); 
    if (context.shouldExit())
        return res;

    return res;
}
