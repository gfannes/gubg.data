#include "catch.hpp"
#include "gubg/data/Table.hpp"
#include "gubg/data/Loader.hpp"
#include <iostream>
#include <vector>
#include <list>
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

    const std::vector<size_t> fixs = {1,0};

    SECTION("vector<vector<>> block")
    {
        std::vector<std::vector<float>> block;
        REQUIRE(table_strange.select<float>(block, fixs));
        for (const auto &row: block)
        {
            for (auto v: row)
                std::cout << v << " ";
            std::cout << std::endl;
        }
    }
    SECTION("list<list<>> block")
    {
        std::list<std::list<float>> block;
        REQUIRE(!table_strange.select<float>(block, fixs));
        block.resize(table_strange.nr_rows());
        REQUIRE(!table_strange.select<float>(block, fixs));
        for (auto &row: block)
            row.resize(fixs.size());
        REQUIRE(table_strange.select<float>(block, fixs));
        for (const auto &row: block)
        {
            for (auto v: row)
                std::cout << v << " ";
            std::cout << std::endl;
        }
    }
}
