#ifndef HEADER_gubg_data_Traits_hpp_ALREADY_INCLUDED
#define HEADER_gubg_data_Traits_hpp_ALREADY_INCLUDED

#include "gubg/Strange.hpp"
#include <string>

namespace gubg { namespace data { 

    template <typename Value> struct Traits
    {
        static bool convert(Value &dst, const gubg::Strange &strange)
        {
            dst = strange;
            return true;
        }
    }; 
    template <> struct Traits<float>
    {
        static bool convert(float &dst, gubg::Strange strange)
        {
            return strange.pop_float(dst);
        }
    };
    template <> struct Traits<std::string>
    {
        static bool convert(std::string &dst, const gubg::Strange &strange)
        {
            dst = strange.str();
            return true;
        }
    };

} } 

#endif
