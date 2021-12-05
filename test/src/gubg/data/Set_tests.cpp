#include "catch.hpp"
#include "gubg/data/Set.hpp"
#include "gubg/s11n/Reader.hpp"

TEST_CASE("data::Set<float> tests", "[ut][data][Set]")
{
    S("");

    struct Scn
    {
        std::string str;
    };
    struct Exp
    {
        bool ok = true;
        std::string name;
        unsigned int nr_fields = 0;
    };

    Scn scn;
    Exp exp;

    SECTION("default ctor")
    {
    }
    SECTION("no name")
    {
        SECTION("empty")
        {
            scn.str = "[:data.Set]{[fields][records]}";
        }
        SECTION("empty1")
        {
            scn.str = "[:data.Set]{[fields]{}[records]{}}";
        }
    }
    SECTION("with name")
    {
        exp.name = "abc";
        SECTION("empty")
        {
            scn.str = "[:data.Set](name:abc){[fields]{}[records]{}}";
        }
        SECTION("two fields, two records")
        {
            exp.nr_fields = 2;
            scn.str = R"%([:data.Set](name:abc){
    [fields]{
        [0](name:x)(dim:2)
        [1](name:y)
    }
    [records]{
        [0]{
            [0]{1 1}
            [1]{42}
        }
        [1]{
            [0]{2 3}
            [1]{43}
        }
    }
    })%";
        }
    }
    SECTION("negative tests")
    {
        exp.ok = false;
        SECTION("missing fields")
        {
            scn.str = "[:data.Set](name:abc){[records]{}}";
        }
        SECTION("missing records")
        {
            scn.str = "[:data.Set](name:abc){[fields]{}}";
        }
        SECTION("missing field name")
        {
            scn.str = "[:data.Set](name:abc){[fields]{[0]}[records]{}}";
        }
        SECTION("field order mismatch")
        {
            scn.str = "[:data.Set](name:abc){[fields]{[1](name:x)}[records]{}}";
        }
        SECTION("record order mismatch")
        {
            scn.str = "[:data.Set](name:abc){[fields]{[0](name:x)}[records]{[1]{42}}}";
        }
        SECTION("not enough values")
        {
            scn.str = "[:data.Set](name:abc){[fields]{[0](name:x)}[records]{[0]{}}}";
        }
        SECTION("too many values")
        {
            scn.str = "[:data.Set](name:abc){[fields]{[0](name:x)}[records]{[0]{0 1}}}";
        }
    }

    using Set = gubg::data::Set<float>;

    Set set;

    if (!scn.str.empty())
    {
        gubg::s11n::Reader r(scn.str);
        const auto ok = r.object(":data.Set", set);
        REQUIRE(ok == exp.ok);
    }

    if (exp.ok)
    {
        REQUIRE(set.fields.size() == exp.nr_fields);
        for (const auto &record: set.records)
        {
            for (auto field: record.fields)
            {
                for (auto v: field)
                    std::cout << v << " ";
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }
}
