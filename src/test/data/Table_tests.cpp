#include "catch.hpp"
#include "gubg/data/Table.hpp"
#include "gubg/data/Loader.hpp"
#include <iostream>
using namespace gubg;

TEST_CASE("data::Table tests", "[ut][data][Table]")
{
    data::Table<gubg::Strange> table_strange;
    data::Loader loader;
    loader.delimiter(' ').has_fieldnames(false);
    REQUIRE(loader(table_strange, "gubg.data/src/test/data.txt"));
    std::cout << table_strange;

    data::Table<float> table_float;
    SECTION("Cannot select the same field twice")
    {
        REQUIRE(!table_strange.select(table_float, {1,1}));
    }
    REQUIRE(table_strange.select(table_float, {1,0}));
    std::cout << table_float;
}
