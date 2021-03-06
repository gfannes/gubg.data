#include "catch.hpp"
#include "gubg/data/Base.hpp"
#include "gubg/s11n/Writer.hpp"
#include <fstream>

TEST_CASE("data::Base<float>", "[ut][data][Base]")
{
    gubg::data::Base<float> db_float;
    REQUIRE(db_float.load("[:data.Set](name:aaa){[fields]{[0](name:x)(dim:2)}[records]{[0]{[0]{1 2}}}}"));
    REQUIRE(db_float.load("[:data.Set](name:bbb){[fields]{[0](name:x)}[records]{[0]{[0]{2}}}}"));

    const auto fn = "data.base.naft";

    {
        std::string str;
        gubg::s11n::Writer<std::string> w(str);
        db_float.write(w);

        std::ofstream fo(fn);
        fo << str;
    }

    gubg::data::Base<std::string> db_string;
    REQUIRE(db_string.load_from_file(fn));
}
