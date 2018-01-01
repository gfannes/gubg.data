#include "catch.hpp"
#include "gubg/data/NominalCounts.hpp"
#include "gubg/data/Nominal.hpp"
#include "gubg/hr.hpp"
#include "gubg/debug.hpp"

namespace  { 
    typedef gubg::data::NominalCounts<3, float> NC;
    typedef gubg::data::Nominal<3> NV;
} 

TEST_CASE("ctor", "[ut][tabledistr]")
{
    S("test");
    NC nc;
    L(gubg::hr(nc.counts()));
}
TEST_CASE("collecting nominal values", "[ut][tabledistr]")
{
    S("test");
    NC nc;
    nc += NV(0);
    nc += NV(1);
    nc += NV(1);
    nc += NV(1);
    nc += NV(2);
    nc += NV(2);
    L(gubg::hr(nc.counts()));
}
TEST_CASE("Summing", "[ut][tabledistr]")
{
    S("test");
    NC nc1;
    {
        nc1 += NV(0);
        nc1 += NV(1);
        nc1 += NV(1);
        nc1 += NV(1);
        nc1 += NV(2);
        nc1 += NV(2);
    }
    NC nc2;
    {
        nc2 += NV(0);
        nc2 += NV(0);
        nc2 += NV(0);
        nc2 += NV(1);
        nc2 += NV(2);
        nc2 += NV(2);
    }
    nc1 += nc2;
    L(gubg::hr(nc1.counts()));
    REQUIRE(12 == nc1.sum());
}
