#include "catch.hpp"
#include "gubg/data/Table.hpp"
#include "gubg/data/Loader.hpp"
#include <iostream>
using namespace gubg;

TEST_CASE("data::Table tests", "[ut][data][Table]")
{
    data::Table<gubg::Strange> table;
    data::Loader loader;
    loader.delimiter(' ').has_fieldnames(false);
    REQUIRE(loader(table, "gubg.data/src/test/data.txt"));
    std::cout << table;
}
