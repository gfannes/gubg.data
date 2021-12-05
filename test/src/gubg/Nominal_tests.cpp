#include "catch.hpp"
#include "gubg/data/Nominal.hpp"
using namespace gubg::data;

namespace  { 
    typedef gubg::data::Nominal<3> NV;
} 

TEST_CASE("Nominal", "[ut][nominal]")
{
    NV nv;
}
